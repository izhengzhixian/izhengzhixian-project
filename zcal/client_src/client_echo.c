#include <gtk/gtk.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define OURPORT htons(1234)
gint sd;
struct sockaddr_in s_in;
gchar username[64];
gchar buf[1024];
gchar get_buf[1048];
gboolean isconnected = FALSE;
static GtkWidget *text;
static GtkTextBuffer *buffer;
static GtkWidget *message_entry;

gboolean do_connect(void)
{
    GtkTextIter iter;
    gint slen;
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        gtk_text_buffer_get_end_iter(buffer, &iter);
        gtk_text_buffer_insert(buffer, &iter, "打开套接字时出错！\n", -1);
        return FALSE;
    }
    //s_in.sin_addr.s_addr = inet_addr("127.0.0.1");
    s_in.sin_family = AF_INET;
    s_in.sin_port = OURPORT;
    slen = sizeof(s_in);
    if (connect(sd, (struct sockaddr*)&s_in, slen) < 0) {
        gtk_text_buffer_get_end_iter(buffer, &iter);
        gtk_text_buffer_insert(buffer, &iter, "连接服务器时出错！\n", -1);
        return FALSE;
    }
    else {
        gtk_text_buffer_get_end_iter(buffer, &iter);
        gtk_text_buffer_insert(buffer, &iter, username, -1);
        gtk_text_buffer_get_end_iter(buffer, &iter);
        gtk_text_buffer_insert(buffer, &iter, "成功与服务器连接...\n", -1);
        isconnected = TRUE;
        return TRUE;
    }
}

void on_send(GtkButton *button, gpointer data)
{
    const char *message;
    GtkTextIter iter;
    if (isconnected == FALSE) return;
    message = gtk_entry_get_text(GTK_ENTRY(message_entry));
    sprintf(buf, "%s\n", message);
    write(sd, buf, 1024);
    gtk_entry_set_text(GTK_ENTRY(message_entry), "");
    gint len = read(sd, buf, 1024);
    buf[len] = 0;
    sprintf(get_buf, "%s\n", buf);
    gtk_text_buffer_get_end_iter(buffer, &iter);
    gtk_text_buffer_insert(buffer, &iter, get_buf, -1);
}

void on_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    close(sd);
    gtk_main_quit();
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *vbox, *hbox, *button, *label, *view;
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(on_delete_event), NULL);
    gtk_window_set_title(GTK_WINDOW(window), "ECHO 客户端");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
    label = gtk_label_new("来自服务器");
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
    view = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(view), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    text = gtk_text_view_new();
    gtk_box_pack_start(GTK_BOX(vbox), view, TRUE, TRUE, 5);
    gtk_container_add(GTK_CONTAINER(view), text);
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));

    hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
    label = gtk_label_new("输入信息：");
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
    message_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox), message_entry, FALSE, FALSE, 5);
    button = gtk_button_new_with_label("发送");
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 5);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_send), NULL);

    do_connect();
    gtk_widget_show_all(window);
    gtk_main();
    return TRUE;
}
