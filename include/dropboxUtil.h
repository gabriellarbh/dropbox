#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "fila2.h"

#include "openssl/ssl.h"
#include "openssl/err.h"
#include <dispatch/dispatch.h>

#define MAXNAME 30
#define MAXFILES 30
#define MAXCHARS 100
#define ERROR 1 
#define SIZE_ERROR 4294967295  /* Que deus perdoe esses programadores ruins */

struct file_info {
	char name[MAXNAME];
  	char extension[MAXNAME];
  	char last_modified[MAXNAME]; 
  	int size;
};
struct client {
	int devices[2];
 	char userid[MAXNAME]; 
 	PFILA2 files;
 	int logged_in;
 };

 typedef struct client CLIENT;
 typedef struct file_info FILEINFO;

int file_size(FILE* fp);

FILEINFO* createFile(char* name,int size);
CLIENT* createClient(char* name);
char* getCurrentTime();

FILEINFO* findFile(CLIENT* user, char* name);
CLIENT* findClient(PFILA2 clientsList, char* name);

int getFileFromStream(char* file, int socket, SSL* ssl);
char* parseFilename(char* src);
int getUnusedFILEINFO(CLIENT user);
char* getPath(CLIENT* user, char*file);

void initSSL();

void parseNameExt(char* src, char* name, char* ext);


