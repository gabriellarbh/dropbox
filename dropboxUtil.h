#define MAXNAME 30

struct client {
	int devices[2];
 	char userid[MAXNAME]; 
 	struct file_info[MAXFILES];
 	int logged_in;
 }

struct file_info {
	char name[MAXNAME];
  	char extension[MAXNAME];
  	char last_modified[MAXNAME]; 
  	int size;
}