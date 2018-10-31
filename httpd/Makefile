all: httpd

httpd: httpd.c
	gcc -std=c99 -W -Wall -lpthread -g -o httpd httpd.c

clean:
	rm httpd
