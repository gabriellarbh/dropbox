#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "server.h"
#include "dropboxUtil.h"

#define PORT 5000

void receive_file(char* file, int socket){
    long int size = 0;
    int count = 0;
    char buffer[256];
    char newBuffer;
    int n;
    unsigned char* bufferSize;
	//FILE* fp = fopen(file, "w+");      <- deve-se usar essa, mas a motivos de teste fiz um novo arquivo.
	FILE* fp = fopen("output.txt", "w+"); 
    if (fp){
        bufferSize = (unsigned char*)&size;
        n = read(socket,bufferSize, 4);

        if(size < 0){
        	printf("File %s doesn't exist\n", file);
        	return;
        }

        while(count < size){
            count++;
            n = read(socket, (void*)&newBuffer, 1);
            fputc(newBuffer,fp);
        }
        
        strcpy(buffer, "File received successfully!\n");
        n = write(socket, buffer, sizeof(buffer));
        printf("File %s finished!!!\n", file);
        fclose(fp);
    }
    else{
        printf("fopen eh null\n");
    }
}

void send_file(char*file, int socket){
	FILE *fp = fopen(file, "r");
	int n, count = 0;
	long int size = file_size(fp);
	char buffer, newBuffer[256];
	// Auxiliar to help casting the file size to buffer
	unsigned char* bufferSize;

	if(!fp) {
		printf("File doesn't exist. Please specify a valid file name\n");
		n = write(socket, "File doesn't exist.", sizeof("File doesn't exist."));
		return;
	}
	// Valid file, starts the stream to the server
	else {
		//printf("Tamanho do arquivo %ld\n", size);  // CUIDAR LITTLE ENDIAN E BIG ENDIAN PQ O ALBERTO VAI RECLAMAR
		// First passes the size of the file to the server
		bufferSize = (unsigned char*) &size;
		n = write(socket, (void*)bufferSize, 4);
		if (n > 0) {
			while(count < size) {
				buffer = fgetc(fp);
				count++;
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
	int n;
	int socket = (int) oldSocket;
	while(1){
        /* read from the socket */
        bzero(buffer, 256);
        n = read(socket, (void*)buffer, 256);
        if (n < 0){
            printf("ERROR reading from socket");
            close(socket);
            return (void*)-1;
        }
        // upload command was received
        if(strstr(buffer, "upload")) {  
        	// Parsing the file name          
            char* fileName = strtok(buffer, " ");
            fileName = strtok(NULL, " ");
            fileName[strlen(fileName) - 1] = 0;
            receive_file(fileName, socket);
	        }
	    else if(strstr(buffer, "download")){
	    	char* fileName = strtok(buffer, " ");
            fileName = strtok(NULL, " ");
            fileName[strlen(fileName) - 1] = 0;
            send_file(fileName, socket);
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
