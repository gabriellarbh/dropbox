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


CLIENT* createClient(char* name) {
	CLIENT* newClient = (CLIENT*) malloc(sizeof(CLIENT));
	strcpy(newClient->userid, name);
	newClient->files = (PFILA2) malloc(sizeof(FILA2));
	CreateFila2(newClient->files);
	newClient->logged_in = 1;
	return newClient;
}

/* Cria o arquivo e seta os valores certinhos  */
FILEINFO* createFile(char* name,int size) {
	FILEINFO* newFile = (FILEINFO*) malloc(sizeof(FILEINFO));
	parseNameExt(name, newFile->name, newFile->extension);
	strcpy(newFile->last_modified, getCurrentTime());
	newFile->size = size;
	return newFile;
}

char* getCurrentTime() {
	time_t rawtime;
  	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	return asctime (timeinfo);
}

FILEINFO* findFile(CLIENT* user, char* name) {
	FirstFila2(user->files);
	FILEINFO* it = (FILEINFO*)GetAtIteratorFila2(user->files);
}

/* Retorna o índice de um FILEINFO ainda não utilizado
 	caso todos estejam full, retorna -1
	MEU DEUS como eu não gosto de array em C
	lista dinâmica é amor <3 */
int getUnusedFILEINFO(CLIENT user){
	return -1;
}

char* parseFilename(char* src) {
	/* Parte 1: A partir do comando, pega só o path */
	char* token;
	char* path = strtok(src, " ");
    path = strtok(NULL, " ");
    path[strlen(path) - 1] = 0;
    /* Com apenas path/to/file/name.ext, ignora o path
       e fica só com o name.ext */
	/* Caso exista um path/to/file, retira */
	if(strstr(path, "/")){
		token = strtok(path, "/");
		while((token) && (!strstr(token, "."))){
			token = strtok(NULL, "/");
		}
		return token;
	}
	/* Caso contrário, retorna path pois ele é só
	   composto por nome e extensão */
	else
		return path;
}

int getFileFromStream(char* file, int socket){
	long int size = 0;
	int i,n;
    unsigned char* bufferSize;
    char buffer;
    /* pega o tamanho do file a ser recebido */
    bufferSize = (unsigned char*)&size;
    n = read(socket,bufferSize, 4);
    if (size >= SIZE_ERROR){
    	return -1;
    }
    else  {
    	FILE* fp = fopen(file, "w+");
	    /* começa a ler da stream, byte a byte, o arquivo */
	    for(i = 0; i < size; i++){
	        n = read(socket, (void*)&buffer, 1);
	        fputc(buffer,fp);
	    }
	    fclose(fp);
	    return size;
	}
}
char* getPath(CLIENT* user, char*file) {
	static char path[MAXCHARS];
    strcpy(path, "sync_dir_");
    strcat(path, user->userid);
    strcat(path, "/");
    strcat(path, file);
    return path;
}

void parseNameExt(char* src, char* name, char* ext){
	strcpy(name, strtok(src, "."));
	strcpy(ext, strtok(NULL, "."));
}






