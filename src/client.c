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
#include "client.h"


#define PORT 5000

 void close_connection(int socket, SSL* ssl) {
 	int n = SSL_write(ssl, "exit", strlen("exit"));
	close(socket);
 }

//Download file from remote directory
void receive_file(char* file, int socket, SSL* ssl){
    int n;
    char buffer[256];
    if(getFileFromStream(file, socket) > 0){
        strcpy(buffer, "File received successfully!\n");
        n = SSL_write(ssl, buffer, sizeof(buffer));
        printf("Download of the file %s finished!\n", file);
    }
    else {
    	printf("File doesn't exist. Tip: Use list to see all the files in your directory ;) \n");
    }
}

//Upload file to remote directory
void send_file(char *file, int socket, SSL* ssl) {
	FILE *fp = fopen(file, "r");
	int n, count = 0;
	long int size = file_size(fp);
	char buffer, newBuffer[256];
	// Auxiliar to help casting the file size to buffer
	unsigned char* bufferSize;

	if(!fp) {
		printf("File doesn't exist. Please specify a valid file name\n");
		bufferSize = (unsigned char*) &size;
		n = SSL_write(ssl, (void*)bufferSize, 4);
		return;
	}
	// Valid file, starts the stream to the server
	else {
		// First passes the size of the file to the server
		bufferSize = (unsigned char*) &size;
		n = SSL_write(ssl, (void*)bufferSize, 4);
		if (n > 0) {
			while(count < size) {
				buffer = fgetc(fp);
				count++;
				n = SSL_write(ssl, (void*)&buffer, 1);
				if (n < 0)
					break;
			}
		}
		//Finished passing the file, closes the file and
		// wait for servers answer
		printf("File %s upload is finished.\n", file);
		fclose(fp);
		n = SSL_read(ssl, newBuffer, 256);
		printf("Server answered: %s\n", newBuffer);
	}
}

void client_loop(int socket, SSL* ssl) {
	int n;
	char* fileName;
    char buffer[256];
	while(1){
    	bzero(buffer, 256);
        printf("client > ");
	    fgets(buffer, 256, stdin);

	    if(strstr(buffer, "exit")){
	    	close_connection(socket, ssl);
	    	break;
	    }
	    if(strstr(buffer, "upload")){
	    	// To get the filename
			n = SSL_write(ssl, buffer, strlen(buffer));
	    	if(n < 0)
	    		printf("Error sending upload command. \n");
	    	else {
		    	// Parsing the file name
		    	fileName = parseFilename(buffer);
				send_file(fileName, socket, ssl);
			}
	    }
	    else if(strstr(buffer, "list")){
	    	n = SSL_write(ssl, buffer, strlen(buffer));
	    	print_file_list(socket, ssl);
	    }
	    else if(strstr(buffer, "download")){
	    	n = SSL_write(ssl, buffer, strlen(buffer));
	    	if(n < 0)
	    		printf("Error sending download command.\n");
	    	else {
		    	fileName = parseFilename(buffer);
				receive_file(fileName, socket, ssl);
			}
	    }
    }
}
// Basicamente o código de TCP do pôfessô
int connect_server(char *host, int port) {
	int socketfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    server = gethostbyname(host);
	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return -1;
    }
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("ERROR opening socket\n");
        return -1;
    }
    
	serv_addr.sin_family = AF_INET;     
	serv_addr.sin_port = htons(port);    
	serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
	bzero(&(serv_addr.sin_zero), 8);    
	if (connect(socketfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        printf("ERROR connecting\n");
    return socketfd;
}

void print_file_list(int socket, SSL* ssl){
	int n;
	char buffer[1000];
		n = SSL_read(ssl, buffer, sizeof(buffer));
		printf("%s", buffer);
}


int main(int argc, char *argv[])
{
	char buffer[MAXCHARS];
    if (argc < 4) {
		printf("Usage: ./client username hostname port");
		exit(0);
    }

    // SSL inits e declaracoes e etc
    initSSL();

    int port = atoi(argv[3]);
    int socket = connect_server(argv[2], port);

    // Depois de ter o socket, faz bind com o SSL
    method = SSLv23_client_method();
	ctx = SSL_CTX_new(method);
	SSL *ssl;
	if (ctx == NULL){
		ERR_print_errors_fp(stderr);
		abort();
	}

	ssl = SSL_new(ctx);
	SSL_set_fd(ssl, socket);

	if (SSL_connect(ssl) == -1){
		printf("ERROR when connecting socket\n");
		ERR_print_errors_fp(stderr);
	}
	else{
  		X509 *cert;
  		char *line;
  		cert = SSL_get_peer_certificate(ssl);
		if (cert != NULL) {
		  line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
		  printf("Subject: %s\n", line);
		  free(line);
		  line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
		  printf("Issuer: %s\n", line);
		}

		// Faz a conexão do usuário com o servidor
		char username[40];
		strcpy(username, "login ");
		strcat(username, argv[1]);
		int n = SSL_write(ssl, username, strlen(username));

		n = SSL_read(ssl, buffer, sizeof(buffer));
		if((n > 0) && (strstr(buffer, "OK")))
			client_loop(socket, ssl);
		else {
			printf("You already have two devices connected. Please, disconnect from one and try again!\n");
		}
			printf("Connection closed. Terminating the program\n");
		return 0;
	}
}
