# libjson
A Simple C Library for Parsing and Editing json files

## Interface functions
```c
void* clonejs(void* node)
```
returns a copy of the json structure or node of the pointer `*node`
```c
char* strjs(void *root);
```
returns a C string representation of the json structure
```c
void  freejs(void *root);
```
frees the memory allocated by the json structure or node in `*root`
```c
void* getjs(void *root, char *key);
```
returns a pointer from the json structure `root` to the node that has a key equals to `key`, or returns `NULL` if it is not found.
```c
void* setjs(void* root, char* key, char* val);
```
returns a copy of the json structure `root` with the value of the node with the key `key` set to `val`.
```c
void* removejs(const void* root, const char* key);
```
returns a copy of the json structure `root` without the node with key `key`.
```c
void* newjs(char* contents);
```
creates and returns a pointer to the json structure parsed from the given c string `contents`.
```c
void* valjs(void *node);
```
returns a void* pointer to the value contained in the node `node`.
```c
void* itemjs(void *node, int i);
```
returns a void* pointer to the node with index `i` from the list in `node`.


## Build and Install
To build the library for linux you need to have `cmake`, `make` and `gcc` installed. Navigate to the `combined\build` folder and run the following commands:
```bash
cmake ..
make all
sudo make install
```
For Windows follow the instructions in the included PDF located in `src_single_file\build\Windows X64\creating-a-c-library-in-visual-studio-2019-bytellect001.pdf`

## Example
```c
#include <stdio.h>
#include <stdlib.h>
#include "json.h"

int main(int argc, char *argv[]){
	char* contents = " {\"string\" : \"hello\" , \"boolean\": true, \"list\": [1, 2, 3], \"object\": { \"null\" : null } } ";
	void* root = newjs(contents);
	if(root != NULL){
		char* cc = strjs(root);
		printf("%s\n", cc);
		free(cc);
		
		void* newroot = setjs(root, "string", "[\"hello\", \"world\"]");
		if(newroot != NULL){
			cc = strjs(newroot);
			printf("%s\n", cc);
			free(cc);
			freejs(newroot);
		}

		freejs(root);
	}
	return 0;
}
```
Compile the example using `gcc` with the following command:
```bash
gcc test.c -o test -lm -ljson
```

## Modifications
The source code is available in the `src_multiple_files` folder. After any modifications, source files can be combined into a single `.c` and `.h` files   using the python script `combine.py` the output is located in the `src_single_file` folder.
