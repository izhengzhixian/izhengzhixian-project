#include "stdio.h"
#include "stdlib.h"
#include "assert.h"
#include "unistd.h"
#include "ctype.h"
#include "string.h"
#include "strings.h"
#include "pthread.h"
#include "fcntl.h"
#include "errno.h"
#include "sys/stat.h"
#include "sys/socket.h"
#include "sys/types.h"
#include "arpa/inet.h"
#include "sys/wait.h"
#include "sys/poll.h"
#include "signal.h"
#include "sys/times.h"
#include "sys/sem.h"

#define DEBUG
#ifdef DEBUG
#define debug(format, ...) fprintf (stderr, format, ## __VA_ARGS__)
#else
#define debug(format, ...)
#endif

#define debug_func() debug("[FUNC] %s: %d\n", __func__, __LINE__)

#define BUF_SIZE 1024
#define THREAD_NUM 8

// 以下类型用来描述用户请求的文件的权限
#define UPM_FILE 1
#define UPM_DIR 2
#define UPM_OTHER 3
#define UPM_CGI 4
typedef struct{
  unsigned char type;
  unsigned char is_readable;
  unsigned char is_writeable;
  unsigned char is_execable;
  char path[128];
}URL_path_mode;

typedef struct {
  char method[8];
  char host[64];
  char user_agent[128];
  char url[64];
  char param[64];
  char http_version[16];
  char content_length[16];
  // variable
  char client_addr[INET_ADDRSTRLEN];
  int client_fd;
  unsigned short client_port;
}HTTP_head_info;

// server config and global variable struct
struct {
  // config
  char server_name[16];
  char server_version[4];
  char log_file_path[24];
  char root_dir[24];
  char cgi_dir[24];
  char *default_index[8];
  char addr[INET_ADDRSTRLEN];
  unsigned short port;
  // variable
  int httpd;
}config_info;

static int thread_sem_id;

// 加载配置文件，用来以后扩展，这里只是简单的对配置修改
int load_config(void);
int create_httpd(int family, char *addr, unsigned short *port);
int handler_connect(int client_fd);
int write_log(char *str, int opt);
int write_http_info_log(HTTP_head_info *hi);
int init_httpd_env(void);
int exit_httpd_env(void);
// 绑定信号退出
void sig_exit_httpd_env(int sig);
// 分析http head, 把相关信息填入HTTP_head_info, 此函数会破坏head
int analysis_head(char * head, HTTP_head_info * hi);
char *get_http_head(char *buffer, int status_code, char *header);
/* 处理get method, 把响应数据包括响应头写入buffer返回，phi 存放保存的hi
 * 返回值：返回写入长度，-1表示出错，如果buffer 写满，返回buf_len
 * 这时需要再次调用get_method(NULL, buffer, buf_len)来获取接下来的数据
 * 没有数据返回则返回0
 * 本函数参考了strtok_r的api
 */
// 参数：要处理的http头部信息
int get_method(HTTP_head_info *hi);
// 参数：要处理的http头部信息，需要readed 指向已经读入的请求体，要求以\0结尾
// 成功返回0, 出错返回-1
int post_method(HTTP_head_info *hi, const char *readed, size_t readed_len);
int write_http_info_log(HTTP_head_info *hi);
/* 路径拼接
 * 如果direct_connect是0, 把path相对root的绝对路径写入root
 * 如果direct_connect非0, 把root连接path的绝对路径写入root
 * 如：path_join("/usr/bin", "/etc/fstab", 0) 返回"/etc/fstab"
 * 如：path_join("/usr/bin", "etc/fstab", 0) 返回"/usr/bin/etc/fstab"
 * 如：path_join("/usr/bin", "/etc/fstab", 1) 返回"/usr/bin/etc/fstab"
 * 如：path_join("/usr/bin", "etc/fstab", 1) 返回"/usr/bin/etc/fstab"
 * 返回值：成功返回0, 出错返回-1, 实际上永远不会出错
 */
int path_join(char *root, const char *path, int direct_connect);
// 把文件内容写入client_fd, 包括响应头
int write_client_file(int client_fd, const char *file_path);
// 把buffer内容写入client_fd, 不包括响应头
int write_client_buffer(int client_fd, const char *buffer, size_t buf_len);
// 根据httpd根目录和用户请求url求出，对应的文件和权限
// 如果是目录，会自动匹配默认index文件，如果找不到，则返回，该目录
// 返回值：存在路径返回0, 不存在返回-1
int httpd_url_path_mode(const char *url, URL_path_mode * upm);
// 执行cgi程序，如果是post, 需要readed参数，表示已经读取的http body，get 可以置NULL
int execute_cgi(HTTP_head_info *hi, const char * cgi_path, const char *readed, size_t readed_len);
void *new_thread_handler(void* pclient_fd);

int main()
{
  load_config();
  init_httpd_env();
  int httpd = create_httpd(AF_INET, config_info.addr, &config_info.port);
  if (httpd < 0){
    perror("create_httpd");
    exit(1);
  }
  config_info.httpd = httpd;
  printf("listen %s:%d\n", config_info.addr, config_info.port);
  struct sockaddr_in client_addr;
  socklen_t client_addr_len;
  int client_fd;
  signal(SIGINT, sig_exit_httpd_env);
  int * pclient_fd;
  struct sembuf thread_sem_dec;
  thread_sem_dec.sem_op = -1;
  thread_sem_dec.sem_num = 0;
  thread_sem_dec.sem_flg = SEM_UNDO;
  while(1){
    // 获取信号，接收client, 生成新的线程
    semop(thread_sem_id, &thread_sem_dec, 1);
    client_fd = accept(httpd, (struct sockaddr*)&client_addr, &client_addr_len);
    // 多线程处理部分
    pclient_fd = malloc(sizeof(int));
    assert(pclient_fd);
    *pclient_fd = client_fd;
    pthread_t pt_id;
    if(pthread_create(&pt_id, NULL, new_thread_handler, pclient_fd)){
      // create pthread error
      exit_httpd_env();
      exit(1);
    }
    pthread_detach(pt_id);

    //handler_connect(client_fd);
  }

  return 0;
}

int load_config(void)
{
  strcpy(config_info.server_name, "z-httpd");
  strcpy(config_info.server_version, "1.0");
  strcpy(config_info.log_file_path, "httpd.log");
  strcpy(config_info.root_dir, "./www");
  strcpy(config_info.cgi_dir, "/cgi");
  config_info.default_index[0] = "index.htm";
  config_info.default_index[1] = "index.html";
  config_info.default_index[2] = NULL;
  strcpy(config_info.addr, "0.0.0.0");
  config_info.port = 8080;
  config_info.httpd = -1;
  return 0;
}

int create_httpd(int family, char *addr, unsigned short *port)
{
  struct sockaddr_in httpd_addr;
  socklen_t httpd_addr_len = sizeof(httpd_addr);
  memset(&httpd_addr, 0, sizeof(httpd_addr));
  httpd_addr.sin_family = family;
  httpd_addr.sin_port = htons(*port);
  int ret = inet_pton(family, addr, &httpd_addr.sin_addr);
  if (ret <= 0) {
    return -1;
  }
  int httpd = socket(family, SOCK_STREAM, 0);
  if (httpd >= 0) {
    int value = 1;
    setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
    ret = bind(httpd, (struct sockaddr*)&httpd_addr, httpd_addr_len);
    if (ret){
      return -1;
    }
    ret = getsockname(httpd, (struct sockaddr*)&httpd_addr, &httpd_addr_len);
    if (ret){
      return -1;
    }
    *port = ntohs(httpd_addr.sin_port);
    listen(httpd, 2);
  }
  return httpd;
}

int handler_connect(int client_fd)
{
  char buffer[BUF_SIZE];
  size_t buf_read_len = 0;
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  struct pollfd fds;
  fds.events = POLLIN;
  fds.fd = client_fd;
  // 正确读取http head标志
  int readed_head_flag = 0;
  int read_num;
  char *http_body;
  HTTP_head_info hhi;
  getpeername(client_fd, (struct sockaddr*)&client_addr, &client_addr_len);
  inet_ntop(AF_INET, &client_addr.sin_addr,
            hhi.client_addr, INET_ADDRSTRLEN);
  hhi.client_port = ntohs(client_addr.sin_port);
  hhi.client_fd = client_fd;
  // 如果3000ms内没有读完http head 的话，就断掉连接
  while(poll(&fds, 1, 3000) == 1){
    read_num = read(client_fd, &buffer + buf_read_len,
                    BUF_SIZE - buf_read_len - 1);
    if(read_num == 0){
      // 客户端关闭套结字
      break;
    }
    buf_read_len += read_num;
    buffer[buf_read_len] = 0;
    char * head_finish = strstr(buffer, "\r\n\r\n");
    if(head_finish == NULL){
      continue;
    }else{
      // 这里把head_finish后移两位，为analysis_head提供便利
      head_finish += 2;
      *head_finish = 0;
      if(analysis_head(buffer, &hhi) == -1){
        // 头部解析错误，直接关闭
        break;
      }
      // 把http body 移动到buffer开头
      http_body = head_finish + 2;
      if(strcmp("GET", hhi.method) == 0){
        int ret = get_method(&hhi);
        if(ret == 0){
          // 解析 get method 成功
          readed_head_flag = 1;
          write_http_info_log(&hhi);
          break;
        }else{
          // 解析get method 出错
          break;
        }
      }else if(strcmp("POST", hhi.method) == 0){
        int ret = post_method(&hhi, http_body, buf_read_len - (http_body - buffer));
        if(ret == 0){
          // 解析 post method 成功
          readed_head_flag = 1;
          write_http_info_log(&hhi);
          break;
        }else{
          // 解析post method 出错
          break;
        }
      }else{
        // 没有实现的method
        get_http_head(buffer, 400, NULL);
        write_client_buffer(client_fd, buffer, strlen(buffer));
      }
      break;
    }
    if(buf_read_len >= BUF_SIZE){
      // 处理不了太长的http头，直接关闭
      break;
    }
  }
  close(client_fd);
  if(readed_head_flag){
    return 0;
  }else{
    return -1;
  }
}

/* 描述：写日志文件
 * 参数：str是要写的字符串，opt是指令
 * opt有效值：'w'表示写日志，此时str有效，'c'表示关闭日志，其他什么也不做，如果文件没被打开，则打开
 * 返回值：正确返回0, 错误返回-1
 */
int write_log(char *str, int opt)
{
  // fwrite是线程安全的，所以写的时候不做任何安全措施
  static FILE * file = NULL;
  if (file == NULL){
    file = fopen(config_info.log_file_path, "a");
    if (!file)
      return -1;
  }
  if(opt == 'w'){
    size_t len = strlen(str);
    printf("%s", str);
    while(len){
      int ret = fwrite(str, 1, len, file);
      len -= ret;
    }
  }else if(opt == 'c'){
    fclose(file);
    file = NULL;
  }
  return 0;
}

int init_httpd_env(void)
{
  // 设置线程数量
  thread_sem_id = semget(ftok(config_info.root_dir, 1), THREAD_NUM, IPC_CREAT);
  if(thread_sem_id < 0){
    exit(1);
  }
  // 打开日志文件
  return write_log(NULL, 'o');
}

int exit_httpd_env(void)
{
  // 关闭日志文件
  close(config_info.httpd);
  config_info.httpd = -1;
  return write_log(NULL, 'c');
}

int analysis_head(char *head, HTTP_head_info * hi)
{
  char *line_end;
  // 对HTTP_head_info初始化
  hi->method[0] = 0;
  hi->http_version[0] = 0;
  hi->url[0] = 0;
  hi->param[0] = 0;
  hi->user_agent[0] = 0;
  hi->content_length[0] = 0;
  char *line_start;
  // 行号
  int line_num = 0;
  for (line_start = head; (line_end = strstr(line_start, "\r\n"));
       line_start = line_end + 2, ++line_num) {
    // 这一行的结尾写入\0, 这样便于处理
    *line_end = 0;
    char *saveptr;
    char *token;
    if(line_num == 0){
      token = strtok_r(line_start, " ", &saveptr);
      // method，如果是小写，转换成大写
      if(token){
        strcpy(hi->method, token);
        for(size_t i = 0; hi->method[i]; ++i) {
          hi->method[i] = toupper(hi->method[i]);
        }
      }
      // url
      if(token){
        token = strtok_r(NULL, " ", &saveptr);
        // 如果存在?则分离url与param
        char * question_mark = strchr(token, '?');
        if(question_mark){
          strcpy(hi->param, question_mark+1);
          *question_mark = 0;
        }
        strcpy(hi->url, token);
      }
      // http版本，如果是小写，转换成大写
      token = strtok_r(NULL, " ", &saveptr);
      if(token){
        strcpy(hi->http_version, token);
        for(size_t i = 0; hi->http_version[i]; ++i) {
          hi->http_version[i] = toupper(hi->http_version[i]);
        }
      }
    }else{
      token = strtok_r(line_start, ":", &saveptr);
      if(token == NULL)
        continue;
      if(strcasecmp("Host", token) == 0){
        size_t space_len = strspn(saveptr, " ");
        strcpy(hi->host, saveptr + space_len);
      }else if(strcasecmp("User-Agent", token) == 0){
        size_t space_len = strspn(saveptr, " ");
        strcpy(hi->user_agent, saveptr + space_len);
      }else if(strcasecmp("Content-length", token) == 0){
        size_t space_len = strspn(saveptr, " ");
        strcpy(hi->content_length, saveptr + space_len);
      }
    }
  }
  return 0;
}

char *get_http_head(char *buffer, int status_code, char *header)
{
  buffer[0] = 0;
  char server[64];
  sprintf(server, "Server: %s/%s\r\n", config_info.server_name, config_info.server_version);
  char str_none[1] = {0};
  header = header? header: str_none;
  switch (status_code) {
  case 200: {
    strcat(buffer, "HTTP/1.0 200 OK\r\n");
    strcat(buffer, server);
    strcat(buffer, "Content-Type: text/html\r\n");
    strcat(buffer, header);
    strcat(buffer, "\r\n");
    break;
  }
  case 400: {
    strcat(buffer, "HTTP/1.0 400 BAD REQUEST\r\n");
    strcat(buffer, server);
    strcat(buffer, "Content-type: text/html\r\n");
    strcat(buffer, header);
    strcat(buffer, "\r\n");
    strcat(buffer, "<P>Your browser sent a bad request, ");
    strcat(buffer, "such as a POST without a Content-Length.\r\n");
    break;
  }
  case 403: {
    strcat(buffer, "HTTP/1.0 403 Forbidden\r\n");
    strcat(buffer, server);
    strcat(buffer, "Content-Type: text/html\r\n");
    strcat(buffer, header);
    strcat(buffer, "\r\n");
    strcat(buffer, "<HTML><TITLE>Forbidden</TITLE>\r\n");
    strcat(buffer, "<BODY><P>Forbidden</P>\r\n");
    strcat(buffer, "</BODY></HTML>\r\n");
    break;
  }
  case 404: {
    strcat(buffer, "HTTP/1.0 404 NOT FOUND\r\n");
    strcat(buffer, server);
    strcat(buffer, "Content-Type: text/html\r\n");
    strcat(buffer, header);
    strcat(buffer, "\r\n");
    strcat(buffer, "<HTML><TITLE>Not Found</TITLE>\r\n");
    strcat(buffer, "<BODY><P>NOT FOUND</P>\r\n");
    strcat(buffer, "</BODY></HTML>\r\n");
    break;
  }
  case 500: {
    strcat(buffer, "HTTP/1.0 500 Internal Server Error\r\n");
    strcat(buffer, server);
    strcat(buffer, "Content-Type: text/html\r\n");
    strcat(buffer, header);
    strcat(buffer, "\r\n");
    strcat(buffer, "<HTML><TITLE>Internal Server Error</TITLE>\r\n");
    strcat(buffer, "<BODY><P>Internal Server Error</P>\r\n");
    strcat(buffer, "</BODY></HTML>\r\n");
    break;
  }
  case 501: {
    strcat(buffer, "HTTP/1.0 501 Method Not Implemented\r\n");
    strcat(buffer, server);
    strcat(buffer, "Content-Type: text/html\r\n");
    strcat(buffer, header);
    strcat(buffer, "\r\n");
    strcat(buffer, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    strcat(buffer, "</TITLE></HEAD>\r\n");
    strcat(buffer, "<BODY><P>HTTP request method not supported.\r\n");
    strcat(buffer, "</BODY></HTML>\r\n");
    break;
  }
  default:
    break;
  }
  return buffer;
}

void sig_exit_httpd_env(int sig)
{
  printf("exit signal: %d\n", sig);
  exit_httpd_env();
  exit(1);
}

int write_http_info_log(HTTP_head_info *hi)
{
  // 写日志
  char log[512];
  if(hi->param[0]){
    sprintf(log, "%s:%u -- \"%s %s?%s %s\" -- %s\n",
            hi->client_addr, hi->client_port, hi->method,
            hi->url, hi->param, hi->http_version, hi->user_agent);
  }else{
    sprintf(log, "%s:%u -- \"%s %s %s\" -- %s\n",
            hi->client_addr, hi->client_port, hi->method,
            hi->url, hi->http_version, hi->user_agent);
  }
  return write_log(log, 'w');
}

int get_method(HTTP_head_info *hi)
{
  char buffer[BUF_SIZE];
  buffer[0] = 0;
  URL_path_mode upm;
  int ret = httpd_url_path_mode(hi->url, &upm);
  if(ret){
    // 不存在对应文件
    get_http_head(buffer, 404, NULL);
  }else{
    if(upm.type == UPM_DIR){
      // 文件夹
      get_http_head(buffer, 403, NULL);
    }else if(upm.type == UPM_OTHER){
      // 其他类型
      get_http_head(buffer, 403, NULL);
    }else if(upm.type == UPM_FILE){
      if(upm.is_readable){
        // 文件可读
        write_client_file(hi->client_fd, upm.path);
        // 防止最后发送buffer给client_fd
        buffer[0] = 0;
      }else{
        // 文件不可读
        get_http_head(buffer, 500, NULL);
      }
    }else if(upm.type == UPM_CGI){
      if(upm.is_execable){
        execute_cgi(hi, upm.path, NULL, 0);
        buffer[0] = 0;
      }else{
        // cgi 文件没有可执行权限
        get_http_head(buffer, 500, NULL);
      }
    }else{
      // 永远不会执行这里
      printf("this is error\n");
    }
  }
  write_client_buffer(hi->client_fd, buffer, strlen(buffer));
  return 0;
}

int post_method(HTTP_head_info *hi, const char *readed, size_t readed_len)
{
  char buffer[BUF_SIZE];
  buffer[0] = 0;
  if(hi->content_length[0] == 0){
    get_http_head(buffer, 400, NULL);
  }else{
    URL_path_mode upm;
    int ret = httpd_url_path_mode(hi->url, &upm);
    if (ret){
      // 路径不存在
      get_http_head(buffer, 404, NULL);
    }else if (upm.type == UPM_CGI && upm.is_execable){
      // 如果cgi文件可以执行，则直接执行
      execute_cgi(hi, upm.path, readed, readed_len);
    }else{
      // 其他情况，报错500
      get_http_head(buffer, 500, NULL);
    }
  }
  write_client_buffer(hi->client_fd, buffer, strlen(buffer));
  return 0;
}

// 返回值： 成功返回0，失败返回-1
int execute_cgi(HTTP_head_info *hi, const char * cgi_path, const char *readed, size_t readed_len)
{
  int cgi_in[2];
  int cgi_out[2];
  char cgi_path_local[128];
  char buffer[BUF_SIZE];
  strcpy(cgi_path_local, cgi_path);
  if (pipe(cgi_in)){
    return -1;
  }
  if (pipe(cgi_out)){
    close(cgi_in[0]);
    close(cgi_in[1]);
    return -1;
  }
  sprintf(buffer, "HTTP/1.0 200 OK\r\n");
  if(write_client_buffer(hi->client_fd, buffer, strlen(buffer))){
    // 客户端关闭
    return 0;
  }
  pid_t pid = fork();
  if(pid < 0)
    return -1;
  else if(pid == 0){
    // 子进程
    close(cgi_in[1]);
    close(cgi_out[0]);
    dup2(cgi_in[0], STDIN_FILENO);
    close(cgi_in[0]);
    dup2(cgi_out[1], STDOUT_FILENO);
    close(cgi_out[1]);
    setenv("REQUEST_METHOD", hi->method, 1);
    setenv("REMOTE_HOST", hi->client_addr, 1);
    setenv("USER_AGENT", hi->user_agent, 1);
    setenv("CONTENT_LENGTH", hi->content_length, 1);
    setenv("QUERY_STRING", hi->param, 1);
    char *cgi_param[] = {cgi_path_local, NULL};
    execv(cgi_path, cgi_param);
  }else{
    // 父进程
    close(cgi_in[0]);
    close(cgi_out[1]);
    // 读取http body 到子进程
    if(strcmp("POST", hi->method) == 0){
      size_t content_length = strtoull(hi->content_length, NULL, 10);
      write_client_buffer(cgi_in[1], readed, readed_len);
      content_length -= strlen(readed);
      while(content_length > 0){
        int ret = read(hi->client_fd, buffer, BUF_SIZE);
        if(ret > 0){
          if(write_client_buffer(cgi_in[1], buffer, ret) == 0){
            // 全部发送子进程
            content_length -= ret;
            continue;
          }
        }
        // 客户端关闭或者出错
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
        close(cgi_in[1]);
        close(cgi_out[0]);
        return -1;
      }
    }
    // 等待子进程退出
    while(1){
      int read_num = read(cgi_out[0], buffer, BUF_SIZE);
      if(read_num <= 0){
        // 子进程关闭
        break;
      }
      if(write_client_buffer(hi->client_fd, buffer, read_num)){
        // 客户端关闭
        kill(pid, SIGKILL);
        break;
      }
    }
    waitpid(pid, NULL, 0);
    close(cgi_in[1]);
    close(cgi_out[0]);
  }
  return 0;
}

int path_join(char *root, const char *path, int direct_connect)
{
  size_t root_len = strlen(root);
  if(root_len){
    if(root[root_len-1] != '/'){
      root[root_len] = '/';
      root[root_len+1] = 0;
    }
  }
  const char *path_walk;
  // path_walk 指向path中第一个不为'/的位置
  for (path_walk=path; *path_walk == '/'; ++path_walk)
    ;
  if(direct_connect == 0 && path_walk != path){
    strcpy(root, path_walk-1);
  }else{
    strcat(root, path_walk);
  }

  return 0;
}

int write_client_file(int client_fd, const char *file_path)
{
  char buffer[BUF_SIZE];
  FILE * file = fopen(file_path, "r");
  if(!file){
    return -1;
  }
  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  if(size < 0){
    fclose(file);
    return -1;
  }
  char content_length[32];
  sprintf(content_length, "Content-length: %d\r\n", size);
  get_http_head(buffer, 200, content_length);
  size_t buf_len = strlen(buffer);
  rewind(file);
  while(buf_len){
    while(buf_len > 0){
      int ret = write(client_fd, buffer, buf_len);
      if(ret <= 0){
        fclose(file);
        return -1;
      }
      buf_len -= ret;
    }
    buf_len = fread(buffer, 1, BUF_SIZE, file);
  }
  fclose(file);
  return 0;
}

int write_client_buffer(int client_fd, const char *buffer, size_t buf_len)
{
  while(buf_len > 0){
    int ret = write(client_fd, buffer, buf_len);
    if(ret <= 0)
      return -1;
    buf_len -= ret;
  }
  return 0;
}

int httpd_url_path_mode(const char *url, URL_path_mode * upm)
{
  strcpy(upm->path, config_info.root_dir);
  // 这个地方存在 .. 可以查看父文件夹内容的bug, 留待以后处理
  path_join(upm->path, url, 1);
  // cgi 文件夹路径
  char cgi_path[128];
  strcpy(cgi_path, config_info.root_dir);
  path_join(cgi_path, config_info.cgi_dir, 1);
  size_t cgi_path_len = strlen(cgi_path);
  struct stat file_stat;
  int ret = stat(upm->path, &file_stat);
  if(ret){
    // 文件不存在
    return -1;
  }else{
    if(S_ISDIR(file_stat.st_mode)){
      // 判断cgi文件夹，如果是，则返回文件不存在，禁止访问cgi文件夹
      if(strcmp(upm->path, cgi_path) >= 0){
        // 该文件的路径开头是cgi目录
        // 如果下一个不同的字符是目录符号或者\0，则是cgi 目录
        if(upm->path[cgi_path_len] == '/' || upm->path[cgi_path_len] == 0)
          return -1;
      }
      // 如果路径是文件夹，则加入default_index
      int default_index_exist = 0;
      // 循环判断索引的path
      char index_path[128];
      for (size_t i = 0; config_info.default_index[i] ; ++i) {
        strcpy(index_path, upm->path);
        path_join(index_path, config_info.default_index[i], 0);
        int ret1 = stat(index_path, &file_stat);
        if(!ret1){
          // 默认文件存在
          default_index_exist = 1;
          strcpy(upm->path, index_path);
          if(S_ISREG(file_stat.st_mode)){
            // 常规文件
            upm->type = UPM_FILE;
          }else if(S_ISDIR(file_stat.st_mode)){
            // 文件夹类型
            upm->type = UPM_DIR;
          }else{
            // 其他类型
            upm->type = UPM_OTHER;
          }
          break;
        }
      }
      if(default_index_exist == 0){
        // 没有默认文件
        upm->type = UPM_DIR;
      }
    }else if(S_ISREG(file_stat.st_mode)){
      // 常规文件
      upm->type = UPM_FILE;
      // 判断cgi文件
      if(strcmp(upm->path, cgi_path) > 0){
        // 该文件的路径开头是cgi目录
        // 如果下一个不同的字符是目录符号，则是cgi 可执行文件
        if(upm->path[cgi_path_len] == '/')
          upm->type = UPM_CGI;
      }
    }else{
      upm->type = UPM_OTHER;
    }
  }
  ret = stat(upm->path, &file_stat);
  if(ret){
    // 永远不会执行这里
    return -1;
  }
  // 以下是权限判断，如果用户，组，其他有一个有权限，则有权限
  if((file_stat.st_mode & S_IRUSR) ||
     (file_stat.st_mode & S_IRGRP) ||
     (file_stat.st_mode & S_IROTH)
     ){
    upm->is_readable = 1;
  }else{
    upm->is_readable = 0;
  }
  if((file_stat.st_mode & S_IWUSR) ||
     (file_stat.st_mode & S_IWGRP) ||
     (file_stat.st_mode & S_IWOTH)
     ){
    upm->is_writeable = 1;
  }else{
    upm->is_writeable = 0;
  }
  if((file_stat.st_mode & S_IXUSR) ||
     (file_stat.st_mode & S_IXGRP) ||
     (file_stat.st_mode & S_IXOTH)
     ){
    upm->is_execable = 1;
  }else{
    upm->is_execable = 0;
  }
  return 0;
}

void *new_thread_handler(void* pclient_fd)
{
  int client_fd = *(int*)pclient_fd;
  handler_connect(client_fd);
  free(pclient_fd);
  // 释放信号
  struct sembuf thread_sem_inc;
  thread_sem_inc.sem_op = 1;
  thread_sem_inc.sem_num = 0;
  thread_sem_inc.sem_flg = SEM_UNDO;
  semop(thread_sem_id, &thread_sem_inc, 1);
  return NULL;
}
