#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define BUFFER_SIZE 4096
#define REMOTE_HOST "127.0.0.1"
#define REMOTE_PORT 1234

typedef struct Sruct_button{
  char *name;
  char *show;
  int x;
  int y;
  void (*callback)(GtkWidget *widget, gpointer point);
}Sbutton;

char * echo_str = NULL;
char * aux_str = NULL;
int echo_str_len = 0;
GtkWidget * label = NULL;
int client_socket = -1;
int user_input_mode = TRUE;
char two_str[2];

int local_create_socket(void);
int local_send_recv(int socket, char *send_buf, char *recv_buf);
void set_show_str(char * str);
void error_handle(char *error_str);
void add_echo_str(GtkWidget *widget, gpointer point);
void control(GtkWidget *widget, gpointer point);
void add_table_button(GtkWidget *table, Sbutton *pbutton);
void key_press_handle(GtkWidget *widget, GdkEventKey *event, gpointer pointer);

Sbutton button_list[] = {
  {"LeftBraket", "(", 1, 0, add_echo_str},
  {"RightBraket", ")", 1, 1, add_echo_str},
  {"Hex", "0x", 1, 2, add_echo_str},
  {"Bin", "0b", 1, 3, add_echo_str},
  {"Abs", "abs", 1, 4, add_echo_str},
  {"ClearEcho", "CE", 2, 0, control},
  {"DelOneChar", "<-", 2, 1, control},
  {"Remain", "%", 2, 2, add_echo_str},
  {"Plus", "+", 2, 3, add_echo_str},
  {"Exp", "exp", 2, 4, add_echo_str},
  {"Seven", "7", 3, 0, add_echo_str},
  {"Eight", "8", 3, 1, add_echo_str},
  {"Nine", "9", 3, 2, add_echo_str},
  {"Minus", "-", 3, 3, add_echo_str},

  {"Pow", "pow", 3, 4, add_echo_str},
  {"Four", "4", 4, 0, add_echo_str},
  {"Five", "5", 4, 1, add_echo_str},
  {"Six", "6", 4, 2, add_echo_str},
  {"Multi", "*", 4, 3, add_echo_str},
  {"Sqrt", "sqrt", 4, 4, add_echo_str},
  {"One", "1", 5, 0, add_echo_str},
  {"Two", "2", 5, 1, add_echo_str},
  {"Three", "3", 5, 2, add_echo_str},
  {"Div", "/", 5, 3, add_echo_str},
  {"Neg", "~", 5, 4, add_echo_str},
  {"Dot", ".", 6, 0, add_echo_str},
  {"Zero", "0", 6, 1, add_echo_str},
  {"And", "&", 6, 2, add_echo_str},
  {"Or", "|", 6, 3, add_echo_str},
  {"Equal", "=", 6, 4, control}
};

char * html_char(char c)
{
	two_str[0] = c;
	switch(c){
		case '&':
			return "&amp;";
		case '<':
			return "&lt;";
		case '>':
			return "&gt;";
		default:
			return two_str;
	}
}



int main(int argc, char **argv)
{
  GtkWidget * window;
  GtkWidget * table;
  GtkWidget * frame;
  echo_str = calloc(sizeof(char), BUFFER_SIZE);
  aux_str = calloc(sizeof(char), BUFFER_SIZE * 2);
  if(! echo_str || ! aux_str){
    printf("calloc error\n");
    exit(1);
  }
  client_socket = local_create_socket();
  gtk_init(&argc, &argv);
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_title(GTK_WINDOW(window), "zcal_client");
  gtk_widget_set_usize(window, 400, 400);
  gtk_container_set_border_width(GTK_CONTAINER(window),10);
  gtk_signal_connect(GTK_OBJECT(window), "destroy", GTK_SIGNAL_FUNC(gtk_main_quit), NULL);

  table = gtk_table_new(7, 5, TRUE);

  int i;
  for(i = 0; i < sizeof(button_list) / sizeof(Sbutton); i++){
    add_table_button(table, button_list + i);
  }

  gtk_container_add(GTK_CONTAINER(window), table);

  label = gtk_label_new(NULL);
  set_show_str("0");
  gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
  gtk_label_set_width_chars(GTK_LABEL(label), 10);
  frame = gtk_frame_new(NULL);
  gtk_container_add(GTK_CONTAINER(frame), label);
  gtk_table_attach_defaults(GTK_TABLE(table), frame, 0, 5, 0, 1);

  gtk_signal_connect(GTK_OBJECT(window), "key_press_event", GTK_SIGNAL_FUNC(key_press_handle), NULL);
  gtk_widget_show_all(window);

  gtk_main();
  free(aux_str);
  free(echo_str);
}

void set_show_str(char * str)
{
  int str_len = strlen(str);

  /* control show str length */
  user_input_mode = FALSE;
  if(user_input_mode && str_len > 20 ){
    str += str_len - 20;
  }
  user_input_mode = TRUE;
  strcpy(aux_str, "<span foreground='blue' font_desc='20'>");
  int i;
  for(i=0; i<str_len; i++){
	  strcat(aux_str, html_char(str[i]));
  }
  strcat(aux_str, "</span>");
  gtk_label_set_markup(GTK_LABEL(label), aux_str);
}

void error_handle(char *error_str)
{
  set_show_str(error_str);
  echo_str[0] = 0;
  echo_str_len = 0;
}

void add_table_button(GtkWidget *table, Sbutton *pbutton)
{
  GtkWidget * button = gtk_button_new_with_label(pbutton->show);
  gtk_signal_connect(GTK_OBJECT(button), "clicked",
                     GTK_SIGNAL_FUNC(pbutton->callback), pbutton->show);
  gtk_table_attach(GTK_TABLE(table), button, pbutton->y, pbutton->y + 1,
		  pbutton->x, pbutton->x + 1, GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 5, 5);

}
void add_echo_str(GtkWidget *widget, gpointer point)
{
  if(echo_str_len + strlen(point) >= BUFFER_SIZE - 1 ){
    error_handle("user input buffer overflow\n");
  }else{
    strcpy(echo_str + echo_str_len, point);
    echo_str_len += strlen(point);
    set_show_str(echo_str);
  }
}
void control(GtkWidget *widget, gpointer point)
{
  if (! strcmp("CE", point)){
    echo_str[0] = 0;
    echo_str_len = 0;
    set_show_str("0");
  }else if(! strcmp("<-", point)){
    if(echo_str_len){
      echo_str_len--;
      echo_str[echo_str_len] = 0;
    }
    if(echo_str_len){
      set_show_str(echo_str);
    }else{
      set_show_str("0");
    }
  }else if(! strcmp("=", point)){
    if(echo_str_len){
      echo_str_len = local_send_recv(client_socket, echo_str, echo_str);
      if(echo_str_len){
        user_input_mode = FALSE;
        set_show_str(echo_str);
      }else{
        close(client_socket);
        printf("Server is break\n");
        exit(1);
      }
      echo_str[0] = 0;
      echo_str_len = 0;
    }
  }else{
    return;
  }
}

int local_create_socket(void)
{
  int local_socket;
  struct sockaddr_in server, client;
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(REMOTE_PORT);
  server.sin_addr.s_addr = inet_addr(REMOTE_HOST);
  local_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(local_socket < 0){
    printf("Create Socket Failed\n");
    exit(1);
  }
  if(connect(local_socket, (struct sockaddr*)&server, sizeof(server))){
    printf("Can Not Connect To %s:%d\n", REMOTE_HOST, REMOTE_PORT);
    exit(1);
  }

  return local_socket;
}

int local_send_recv(int local_socket, char *send_buf, char *recv_buf)
{
  send(local_socket, send_buf, strlen(send_buf), 0);
  int recv_len = recv(local_socket, recv_buf, BUFFER_SIZE-1, 0);
  recv_buf[recv_len] = 0;
  return recv_len;
}

void key_press_handle(GtkWidget *widget, GdkEventKey *event, gpointer pointer)
{
  two_str[2] = event->keyval;
  if(isgraph(event->keyval))
    add_echo_str(NULL, two_str);
  else if(event->keyval == GDK_KEY_BackSpace)
    control(NULL, "<-");
  else if(event->keyval == GDK_KEY_Return)
    control(NULL, "=");
}
