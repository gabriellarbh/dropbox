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
