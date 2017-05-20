#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 5000

void receive_file(char* file, int newsockfd){
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
        n = read(newsockfd,bufferSize, 4);
        while(count < size){
            count++;
            n = read(newsockfd, (void*)&newBuffer, 1);
            fputc(newBuffer,fp);
        }
        
        strcpy(buffer, "File received successfully!\n");
        n = write(newsockfd, buffer, sizeof(buffer));
        printf("File %s finished!!!\n", file);
        fclose(fp);
    }
    else{
        printf("fopen eh null\n");
    }

}

void server_loop(int socket){
	char buffer[256];
	int n;
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
            receive_file(fileName, socket);
	        }
	    else if (strstr(buffer, "close")){
	    	printf("Closing client with id %d\n", socket);
    		close(socket);
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
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        printf("ERROR opening socket");
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        printf("ERROR on binding");
    
    listen(sockfd, 5);
    
    clilen = sizeof(struct sockaddr_in);
    
    if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) == -1)
        printf("ERROR on accept");
    server_loop(newsockfd);
    close(sockfd);
    return 0;
}
