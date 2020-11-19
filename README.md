# libjson
A Library for Parsing and Editing json files in C

## API
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
To build the library you need to have `cmake`, `make` and `gcc` installed. Navigate to the `combined\build` folder and run the following commands:
```bash
cmake ..
make all
sudo make install
```

## Modifications
The source code is available in the `seperated` folder. After any modifications, source files can be combined into a single `.c` and `.h` files   using the python script `combiner.py` the output is located in the `combined` folder.
