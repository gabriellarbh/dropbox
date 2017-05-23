#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "server.h"
#define PORT 5000

void receive_file(char* file, int socket, CLIENT user){
    long int size = 0;
    int i;
    char buffer[256];
    char newBuffer;
    int n;
    unsigned char* bufferSize;
    char *path = getPath(user, file);
	FILE* fp = fopen(path, "r+");    //  <- deve-se usar essa, mas a motivos de teste fiz um novo arquivo.
	//FILE* fp = fopen("output.txt", "w+"); 

    if (fp){
        fclose(fp);
        if(getFileFromStream(path, socket)) {
            // Avisa o cliente que o file foi atualizado
            // e coloca mensagem na tela
            strcpy(buffer, "File updated successfully!\n");
            n = write(socket, buffer, sizeof(buffer));
            printf("User %s id %d > File %s finished!!!\n", user.userid,socket,file);

            // Como o arquivo existe, a estrutura FILEINFO também já existe
            // logo, a atualiza com o novo time stamp
            FILEINFO aux = findFile(user, file);
            strcpy(aux.last_modified, getCurrentTime());
            printf("User %s id %d > File %s updated at %s\n",user.userid,socket, aux.name, aux.last_modified);
        }
        else {
            strcpy(buffer, "Error while transfering file. Please, try again\n");
            n = write(socket, buffer, sizeof(buffer));
        } 
    }
    // File não existe no servidor. Cria o arquivo e a estrutura
    else{
        if(getFileFromStream(path, socket)){
            strcpy(buffer, "File uploaded successfully!\n");
            n = write(socket, buffer, sizeof(buffer));
            printf("User %s id %d > File %s upload finished!!!\n", user.userid, socket, file);

            FILEINFO aux = user.files[getUnusedFILEINFO(user)];
            strcpy(aux.name, file);
            strcpy(aux.extension, "txt");
            strcpy(aux.last_modified, getCurrentTime());
            aux.size = size;
        }
        else {
            strcpy(buffer, "Error while transfering file. Please, try again\n");
            n = write(socket, buffer, sizeof(buffer));
        }
    }
}


// AGORA: Cria o diretório do usuário, caso ele não exista ainda
// Depois: Cria o diretório e a estrutura cliente e a preenche com os arquivos dentro
// do diretório
struct client login_user(char* user){
    struct stat st = {0};
    char directory[40];
    strcpy(directory, "./sync_dir_");
    strcat(directory, user);
    if (stat(directory, &st) == -1) {
        mkdir(directory, 0700);
    }
    struct client novo = createClient(user);
    return novo;

}

void send_file(char*file, int socket, CLIENT user){
    char *path = getPath(user, file);
	FILE *fp = fopen(path, "r");
	int n, i;
	long int size = file_size(fp);
	char buffer, newBuffer[256];
	// Auxiliar to help casting the file size to buffer
	unsigned char* bufferSize;

	if(fp == NULL) {
		printf("File doesn't exist. Please specify a valid file name %ld \n", size);
        bufferSize = (unsigned char*) &size;
        n = write(socket, (void*)bufferSize, 4);
        if (n < 0)
            printf("Deu erro na conexão");
		return;
	}
	// Valid file, starts the stream to the server
	else {
		//printf("Tamanho do arquivo %ld\n", size);  // CUIDAR LITTLE ENDIAN E BIG ENDIAN PQ O ALBERTO VAI RECLAMAR
		// First passes the size of the file to the server
		bufferSize = (unsigned char*) &size;
		n = write(socket, (void*)bufferSize, 4);
		if (n > 0) {
			for(i = 0; i < size; i++) {
				buffer = fgetc(fp);
				n = write(socket, (void*)&buffer, 1);
				if (n < 0)
					break;
			}
		}
		//Finished passing the file, closes the file and
		// wait for servers answer
		printf("File %s upload is finished.\n", file);
		fclose(fp);
		n = read(socket, newBuffer, 256);
		printf("Client answered: %s\n", newBuffer);
	}

}

void* server_loop(void *oldSocket){
	char buffer[256];
    char* fileName;
	int n;
	int socket = (int) oldSocket;
	CLIENT user;
	while(1){
        /* read from the socket */
        bzero(buffer, 256);
        n = read(socket, (void*)buffer, 256);
        if (n < 0){
            printf("ERROR reading from socket");
            close(socket);
            return (void*)-1;
        }
        if(strstr(buffer, "login")){
            char* username = strtok(buffer, " ");
            username = strtok(NULL, " ");
            user = login_user(username);
        }
        else if(strstr(buffer, "upload")) {   
            fileName = parseFilename(buffer);
            receive_file(fileName, socket, user);
	        }
	    else if(strstr(buffer, "download")){
	    	fileName = parseFilename(buffer);
            send_file(fileName, socket, user);
	    }
	    else if (strstr(buffer, "exit")){
	    	printf("Closing client with id %d\n", socket);
    		close(socket);
    		pthread_exit(0);
    		break;
	    }

	}
	return (void*) 1;
}


int main(int argc, char *argv[])
{
    int sockfd, newsockfd, n;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    pthread_t tid[10];
    int threadCount = 0;
    //pid_t pid;
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        printf("ERROR opening socket\n");
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        printf("ERROR on binding\n");
    while(1){
	    listen(sockfd, 5);
	    clilen = sizeof(struct sockaddr_in);
	    if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) == -1){
	        printf("ERROR on accept\n");
	    }
	    else {
	    	pthread_create(&tid[threadCount], NULL, server_loop, (void*)(size_t)newsockfd);
	    	threadCount++;
    		pthread_join(tid[threadCount], NULL);
	    }
	}
    close(sockfd);
    return 0;
}
