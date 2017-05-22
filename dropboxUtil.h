
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>

#define MAXNAME 30
#define MAXFILES 30
struct file_info {
	char name[MAXNAME];
  	char extension[MAXNAME];
  	char last_modified[MAXNAME]; 
  	int size;
};
struct client {
	int devices[2];
 	char userid[MAXNAME]; 
 	struct file_info files[MAXFILES];
 	int logged_in;
 };



int file_size(FILE* fp);
