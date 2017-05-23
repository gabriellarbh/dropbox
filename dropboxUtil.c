#include "dropboxUtil.h"

int file_size(FILE *fp){
	int returnSize;
	if(fp != NULL){
	    int prev=ftell(fp);
	    fseek(fp, 0L, SEEK_END);
	    returnSize = ftell(fp);
	    fseek(fp,prev,SEEK_SET);
	    return returnSize;
    }
    else {
    	return -1;
    }
}


struct client createClient(char* name) {
	int i;
	struct client newClient;
	strcpy(newClient.userid,name);
	for(i = 0; i < MAXFILES; i++){
		newClient.files[i] = createFile("", "", -1);
	}
	newClient.logged_in = 1;
	return newClient;
}

/* Cria o arquivo e seta os valores certinhos 
PONTEIROS ME BEIJEM :( */
struct file_info createFile(char* name, char* ext, int size) {
	struct file_info newFile;
	strcpy(newFile.name,name);
	strcpy(newFile.extension,ext);
	strcpy(newFile.last_modified, getCurrentTime());
	newFile.size = size;
	return newFile;
}

char* getCurrentTime() {
	time_t rawtime;
  	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	return asctime (timeinfo);
}

struct file_info findFile(struct client user, char* name, char* ext) {
	int count = 0;
	struct file_info it = user.files[0];
	while((count < MAXFILES) && (it.size > 0)) {
		if((strcmp(it.name, name) == 0) && (strcmp(it.extension, ext) == 0))
			return it;
		count++;
		it = user.files[count];
	}
	return it;
}
