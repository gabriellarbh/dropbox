############################# Makefile ##########################
all: client server
client: ./src/client.c dropbox.a ./include/dropboxUtil.h
        # O compilador faz a ligação entre os dois objetos
	gcc -o client ./src/client.c -I./include -L./lib -ldropbox

server: ./src/server.c dropbox.a ./include/dropboxUtil.h ./include/fila2.h
        # O compilador faz a ligação entre os dois objetos
	gcc -o server ./src/server.c -I./include -L./lib -ldropbox
#-----> Distancia com o botão TAB ### e não com espaços

dropbox.a: fila2.o dropboxUtil.o
	ar crs ./lib/libdropbox.a dropboxUtil.o fila2.o

dropboxUtil.o: ./src/dropboxUtil.c ./include/dropboxUtil.h 
	gcc -c -I./include ./src/dropboxUtil.c

fila2.o: ./src/fila2.c ./include/fila2.h
	gcc -c -I./include ./src/fila2.c 

clean:
	rm *.o server client
	rm lib/*.a
cleanTXT:
	rm -rf *.txt
