#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 5000

int clientSocket = -1;
int fsize(FILE *fp){
	if(fp != NULL){
	    int prev=ftell(fp);
	    fseek(fp, 0L, SEEK_END);
	    int sz=ftell(fp);
	    fseek(fp,prev,SEEK_SET); //go back to where we were
	    return sz;
    }
    else {
    	printf("FILE NAO EXISTE SUA ANIMALE\n");
    	return -1;
    }
}

void receive_file(char* file){
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
        n = read(clientSocket,bufferSize, 4);
        while(count < size){
            count++;
            n = read(clientSocket, (void*)&newBuffer, 1);
            fputc(newBuffer,fp);
        }
        
        strcpy(buffer, "File received successfully!\n");
        n = write(clientSocket, buffer, sizeof(buffer));
        printf("File %s finished!!!\n", file);
        fclose(fp);
    }
    else{
        printf("fopen eh null\n");
    }
}

void send_file(char*file){
	FILE *fp = fopen(file, "r");
	int n, count = 0;
	long int size = fsize(fp);
	char buffer, newBuffer[256];
	// Auxiliar to help casting the file size to buffer
	unsigned char* bufferSize;

	if(!fp) {
		printf("File doesn't exist. Please specify a valid file name\n");
		n = write(clientSocket, "File doesn't exist.", sizeof("File doesn't exist."));
		return;
	}
	// Valid file, starts the stream to the server
	else {
		//printf("Tamanho do arquivo %ld\n", size);  // CUIDAR LITTLE ENDIAN E BIG ENDIAN PQ O ALBERTO VAI RECLAMAR
		// First passes the size of the file to the server
		bufferSize = (unsigned char*) &size;
		n = write(clientSocket, (void*)bufferSize, 4);
		if (n > 0) {
			while(count < size) {
				buffer = fgetc(fp);
				count++;
				n = write(clientSocket, (void*)&buffer, 1);
				if (n < 0)
					break;
			}
		}
		//Finished passing the file, closes the file and
		// wait for servers answer
		printf("File %s upload is finished.\n", file);
		fclose(fp);
		n = read(clientSocket, newBuffer, 256);
		printf("Client answered: %s\n", newBuffer);
	}

}

void server_loop(int socket){
	char buffer[256];
	int n;
	clientSocket = socket;
	while(1){
        /* read from the socket */
        bzero(buffer, 256);
        
        n = (int)read(socket, (void*)buffer, 256);
        if (n < 0)
            printf("ERROR reading from socket");
        // upload command was received
        if(strstr(buffer, "upload")) {  
        	// Parsing the file name          
            char* fileName = strtok(buffer, " ");
            fileName = strtok(NULL, " ");
            fileName[strlen(fileName) - 1] = 0;
            receive_file(fileName);
	        }
	    else if(strstr(buffer, "download")){
	    	char* fileName = strtok(buffer, " ");
            fileName = strtok(NULL, " ");
            fileName[strlen(fileName) - 1] = 0;
            send_file(fileName);
	    }
	    else if (strstr(buffer, "exit")){
	    	printf("Closing client with id %d\n", clientSocket);
    		close(clientSocket);
    		break;
	    }

	}

}


int main(int argc, char *argv[])
{
    int sockfd, newsockfd, n;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    pid_t pid;
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        printf("ERROR opening socket");
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        printf("ERROR on binding");
    while(1){
	    listen(sockfd, 5);
	    clilen = sizeof(struct sockaddr_in);
	    if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) == -1){
	        printf("ERROR on accept");
	    }
	    else {
	    	pid = fork();
	    	if(pid == 0) {
	    		server_loop(newsockfd);
	    	}
	    }
	}
    close(sockfd);
    return 0;
}
