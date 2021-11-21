#include <stdio.h>
#include <stdlib.h>
#include "json.h"

char* read_file(char *filename){
   char *buffer = NULL;
   size_t string_size, read_size;
   FILE* stream;
   errno_t error = fopen_s(&stream, filename, "r");

   if (error == 0){
       fseek(stream, 0, SEEK_END);
       string_size = ftell(stream);
       string_size++;
       rewind(stream);
       buffer = (char*) malloc(sizeof(char) * (string_size) );
       if (buffer == NULL) return NULL;
       read_size = fread(buffer, sizeof(char), string_size, stream);
       buffer[string_size-1] = '\0';
       if (string_size != read_size){
           free(buffer);
           buffer = NULL;
       }
       fclose(stream);
    }
    return buffer;
}

int main(int argc, char *argv[]){
	if(argc == 2){
		char* contents = read_file(argv[1]);
		if(contents != NULL){
			void *root = newjs(contents);
			if(root != NULL){
                freejs(root);
                free(contents);
                return 0;
        	}
            free(contents);
		}
	}else printf("no argument given\n");
	return 1;
}
