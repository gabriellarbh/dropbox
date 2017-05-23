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


CLIENT createClient(char* name) {
	int i;
	CLIENT newClient;
	strcpy(newClient.userid,name);
	for(i = 0; i < MAXFILES; i++){
		newClient.files[i] = createFile("", "", -1);
	}
	newClient.logged_in = 1;
	return newClient;
}

/* Cria o arquivo e seta os valores certinhos 
PONTEIROS ME BEIJEM :( */
FILEINFO createFile(char* name, char* ext, int size) {
	FILEINFO newFile;
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

FILEINFO findFile(CLIENT user, char* name) {
	int count = 0;
	FILEINFO it = user.files[0];
	while((count < MAXFILES) && (it.size > 0)) {
		if(strstr(name, it.name) && (strstr(name, it.extension)))
			return it;
		count++;
		it = user.files[count];
	}
	return it;
}

/* Retorna o índice de um FILEINFO ainda não utilizado
 	caso todos estejam full, retorna -1
	MEU DEUS como eu não gosto de array em C
	lista dinâmica é amor <3 */
int getUnusedFILEINFO(CLIENT user){
	int i;
	for(i = 0; i < MAXFILES; i++){
		if(user.files[i].size < 0)
			return i;
	}
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
	FILE* fp = fopen(file, "w+");
	long int size = 0;
	int i,n;
    unsigned char* bufferSize;
    char buffer;
    /* pega o tamanho do file a ser recebido */
    bufferSize = (unsigned char*)&size;
    n = read(socket,bufferSize, 4);
    if (size < 0){
    	printf("DEU ERRO GORIZADA");
    	return -1;
    }
    /* começa a ler da stream, byte a byte, o arquivo */
    for(i = 0; i < size; i++){
        n = read(socket, (void*)&buffer, 1);
        fputc(buffer,fp);
    }
    fclose(fp);
    return 1;
}
char* getPath(CLIENT user, char*file) {
	static char path[MAXCHARS];
    strcpy(path, "sync_dir_");
    strcat(path, user.userid);
    strcat(path, "/");
    strcat(path, file);
    return path;
}







