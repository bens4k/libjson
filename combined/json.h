#ifndef JSON_H
#define JSON_H


void* clonejs(void* node);
char* strjs(void *root);
void  freejs(void *root);
void* getjs(void *root, char *key);
void* setjs(void* root, char* key, char* val);
void* removejs(const void* root, const char* key);
void* newjs(char* contents);
void* valjs(void *node);
// get the item with index i from the list in node
void* itemjs(void *node, int i);
#endif
