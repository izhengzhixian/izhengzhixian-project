client: client_cli client_echo client_gtk
client_cli: client_cli.c
	gcc -o client_cli client_cli.c
client_echo: client_echo.c
	gcc -o client_echo client_echo.c `pkg-config --libs --cflags gtk+-2.0`
client_gtk: client_gtk.c
	gcc -o client_gtk client_gtk.c `pkg-config --libs --cflags gtk+-2.0`
