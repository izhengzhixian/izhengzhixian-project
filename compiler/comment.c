#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NORMAL 0
#define MULTI_COMMENT 1
#define SINGLE_COMMENT 2
#define SPACE 3

int set_buf(char ** p, size_t * bufsize, size_t index, char value);

int main(int argc, const char *argv[])
{
  char * buffer = malloc(BUFSIZ);
  char * out = malloc(BUFSIZ);
  assert(buffer);
  assert(out);

  FILE * f = fopen("main.c", "r");

  size_t len;
  /*上一次的扫描字符，在扫描文件中不会出现0这个值，所以可以拿0做特殊值
   *表示不能对后续扫描产生影响
   */
  char previous = 0;
  int status = NORMAL;
  size_t out_len = BUFSIZ;
  size_t out_index = 0;
  size_t buf_index = 0;
  while((len = fread(buffer, 1, BUFSIZ-1, f)) != 0){
    for (buf_index = 0; buf_index < len; buf_index++) {
      char current = buffer[buf_index];
      switch(status){
      case NORMAL:
        if(previous == '/' && current == '/'){
          status = SINGLE_COMMENT;
          //让上一次放入输出缓冲区的'/'弹出
          out_index--;
          previous = 0;
        }else if(previous == '/' && current == '*'){
          status = MULTI_COMMENT;
          //让上一次放入输出缓冲区的'/'弹出
          out_index--;
          previous = 0;
        }else if(current == ' ' || current == '\t' || current == '\n'){
          status = SPACE;
          previous = 0;
        }else{
          set_buf(&out, &out_len, out_index, current);
          out_index++;
          previous = current;
        }
        break;
      case MULTI_COMMENT:
        if(previous == '*' && current == '/'){
          previous = 0;
          status = SPACE;
        }else{
          previous = current;
        }
        break;
      case SINGLE_COMMENT:
        if(current == '\n'){
          status = SPACE;
        }
        break;
                
      case SPACE:
        if(current != ' ' && current != '\t' && current != '\n'){
          status = NORMAL;
          while(out_index && out[out_index-1] == ' '){
            out_index--;
          }
          set_buf(&out, &out_len, out_index, ' ');
          out_index++;
          buf_index--;
        }
        break;

      default:
        printf("status error\n");
        exit(1);
      }
    }
  }

  printf("%s", out);

  free(out);
  free(buffer);
  fclose(f);

  return 0;
}

int set_buf(char ** p, size_t * bufsize, size_t index, char value)
{
  if(index >= *bufsize){
    *p = realloc(*p, *bufsize + BUFSIZ);
    assert(*p);
    *bufsize = *bufsize + BUFSIZ;
  }
  (*p)[index] = value;
  return 0;
}
