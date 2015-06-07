all:
	gcc -o gethar gethar2.c $(pkg-config --cflags --libs webkit2gtk-4.0) -Wall -O1 -g

