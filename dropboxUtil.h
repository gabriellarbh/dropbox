#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
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

struct file_info createFile(char* name, char* ext, int size);
struct client createClient(char* name);
char* getCurrentTime();

struct file_info findFile(struct client user, char* name, char* ext);
