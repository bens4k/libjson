#include <stdio.h>
#include <stdlib.h>
#include "json.h"

char* read_file(char *filename){
   char *buffer = NULL;
   int string_size, read_size;
   FILE *handler = fopen(filename, "r");

   if (handler)
   {
       fseek(handler, 0, SEEK_END);
       string_size = ftell(handler);
       rewind(handler);
       buffer = (char*) malloc(sizeof(char) * (string_size + 1) );
       read_size = fread(buffer, sizeof(char), string_size, handler);
       buffer[string_size] = '\0';
       if (string_size != read_size){
           free(buffer);
           buffer = NULL;
       }
       fclose(handler);
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
