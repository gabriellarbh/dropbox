#Para escrever comentários ##
############################# Makefile ##########################
all: client server
client: client.o dropboxUtil.o 
        # O compilador faz a ligação entre os dois objetos
	gcc -o client dropboxUtil.o client.o
server: server.o dropboxUtil.o 
        # O compilador faz a ligação entre os dois objetos
	gcc -o server dropboxUtil.o server.o
#-----> Distancia com o botão TAB ### e não com espaços
dropboxUtil.o: dropboxUtil.c
	gcc -o dropboxUtil.o -c dropboxUtil.c -W -Wall -ansi -pedantic
clean:
	rm -rf *.o
cleanTXT:
	rm -rf *.txt
