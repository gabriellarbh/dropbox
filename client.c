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



int fsize(FILE *fp){
	if(fp != NULL){
	    int prev=ftell(fp);
	    fseek(fp, 0L, SEEK_END);
	    int sz=ftell(fp);
	    fseek(fp,prev,SEEK_SET); //go back to where we were
	    return sz;
    }
    else {
    	return -1;
    }
}
void receive_file(char* file, int socket){
	long int size = 0;
    int count = 0;
    char buffer[256];
    char newBuffer;
    int n;
    unsigned char* bufferSize;
	FILE* fp = fopen(file, "w+"); // <- tem que cuidar saporra aqui
    if (fp){
        bufferSize = (unsigned char*)&size;
        n = read(socket,bufferSize, 4);
        while(count < size){
            count++;
            n = read(socket, (void*)&newBuffer, 1);
            fputc(newBuffer,fp);
        }
        
        strcpy(buffer, "File received successfully!\n");
        n = write(socket, buffer, sizeof(buffer));
        printf("Download of the file %s finished!!!\n", file);
        fclose(fp);
    }
    else{
        printf("fopen eh null\n");
    }
}

void send_file(char *file, int socket) {
	FILE *fp = fopen(file, "r");
	int n, count = 0;
	long int size = fsize(fp);
	char buffer, newBuffer[256];
	// Auxiliar to help casting the file size to buffer
	unsigned char* bufferSize;

	if(!fp) {
		printf("File doesn't exist. Please specify a valid file name\n");
		bufferSize = (unsigned char*) &size;
		n = write(socket, (void*)bufferSize, 4);
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
		printf("Server answered: %s\n", newBuffer);
	}
}

void client_loop(int socket) {
	 int n;
	
    char buffer[256];
	while(1){
    	bzero(buffer, 256);
	    printf("client > ");
	    fgets(buffer, 256, stdin);

	    if(strstr(buffer, "exit")){
	    	n = write(socket, buffer, strlen(buffer));
	    	close(socket);
	    	break;
	    }
	    if(strstr(buffer, "upload")){
	    	// To get the filename
			n = write(socket, buffer, strlen(buffer));
	    	if(n < 0)
	    		printf("Error sending upload command. Please try again.(?)\n");
	    	// Parsing the file name
	    	char* fileName = strtok(buffer, " ");
	    	fileName = strtok(NULL, " ");
	    	fileName[strlen(fileName) - 1] = 0;
			send_file(fileName, socket);
	    }
	    else if(strstr(buffer, "download")){
	    	n = write(socket, buffer, strlen(buffer));
	    	if(n < 0)
	    		printf("Error sending download command.\n");
	    	char* fileName = strtok(buffer, " ");
	    	fileName = strtok(NULL, " ");
	    	fileName[strlen(fileName) - 1] = 0;
			receive_file(fileName, socket);
	    }
    }
}

int main(int argc, char *argv[])
{
    int socketfd, n;
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
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        printf("ERROR opening socket\n");
    
	serv_addr.sin_family = AF_INET;     
	serv_addr.sin_port = htons(PORT);    
	serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
	bzero(&(serv_addr.sin_zero), 8);    
	if (connect(socketfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        printf("ERROR connecting\n");

    client_loop(socketfd);
    
   	printf("Connection closed. Terminating the program\n");
    return 0;
}