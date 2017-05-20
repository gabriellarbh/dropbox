#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h> 

#define PORT 5000

int globalSocket;


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


void send_file(char *file) {
	FILE *fp = fopen(file, "r");
	int n, count = 0;
	long int size = fsize(fp);
	char buffer, newBuffer[256];
	// Auxiliar to help casting the file size to buffer
	unsigned char* bufferSize;

	if(!fp) {
		printf("File doesn't exist. Please specify a valid file name\n");
		return;
	}
	// Valid file, starts the stream to the server
	else {
		//printf("Tamanho do arquivo %ld\n", size);  // CUIDAR LITTLE ENDIAN E BIG ENDIAN PQ O ALBERTO VAI RECLAMAR
		// First passes the size of the file to the server
		bufferSize = (unsigned char*) &size;
		n = write(globalSocket, (void*)bufferSize, 4);
		if (n > 0) {
			while(count < size) {
				buffer = fgetc(fp);
				count++;
				n = write(globalSocket, (void*)&buffer, 1);
				if (n < 0)
					break;
			}
		}
		//Finished passing the file, closes the file and
		// wait for servers answer
		printf("File %s upload is finished.\n", file);
		fclose(fp);
		n = read(globalSocket, newBuffer, 256);
		printf("Server answered: %s\n", newBuffer);
	}
}

int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
	
    char buffer[256];
    if (argc < 2) {
		fprintf(stderr,"usage %s hostname\n", argv[0]);
		exit(0);
    }
	
	server = gethostbyname(argv[1]);
	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        printf("ERROR opening socket\n");
    
	serv_addr.sin_family = AF_INET;     
	serv_addr.sin_port = htons(PORT);    
	serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
	bzero(&(serv_addr.sin_zero), 8);    
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        printf("ERROR connecting\n");

    // ATÉ AQUI É CÓDIGO DO ALBERTO
    
    while(1){
    	globalSocket = sockfd;
    	bzero(buffer, 256);
	    printf("client > ");
	    fgets(buffer, 256, stdin);

	    if(strstr(buffer, "close")){
	    	n = write(sockfd, buffer, strlen(buffer));
	    	close(sockfd);
	    	break;
	    }
	    if(strstr(buffer, "upload")){
	    	// To get the filename
			n = write(sockfd, buffer, strlen(buffer));
	    	if(n < 0)
	    		printf("Error sending upload command. Please try again.(?)\n");
	    	// Parsing the file name
	    	char* fileName = strtok(buffer, " ");
	    	fileName = strtok(NULL, " ");
	    	fileName[strlen(fileName) - 1] = 0;
			send_file(fileName);
	    }
    }
   	printf("Connection closed. Terminating the program\n");
    return 0;
}