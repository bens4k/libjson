#include "../include/json.h"

/*start of func defs*/
// returns a new js tree with key = val , if exists change it, if not add it
void* setjs(void* root, char* key, char* val){
	Garbage* gr = new_garbage();
	// construct {key: val} 
	CString* keycs = w(gr, new_cstring_str(key), GrCString);
	CString* contents = w(gr, enclose(keycs, '\"', '\"'), GrCString);
	contents = w(gr, joinstr(contents, ":", 0), GrCString);
	contents = w(gr, joinstr(contents, val, 0), GrCString);
	contents = w(gr, enclose(contents, '{', '}'), GrCString);
	// parse
	TokensList* tknlist = w(gr, tokenize(contents), GrTokensList);
	ParserReport* rep = w(gr, new_report(), GrParsingReport);
	Node* nwrb = w(gr, parse(tknlist, rep), GrNode);
	Node* n = clone_node(((Block*)(nwrb->value))->first_child);
	// if already exist, remove old and set new
	Node* old_parent = find_parent(root, keycs);
	Node* newtree = removejs(root, key);
	Node* new_parent = newtree;
	if((old_parent != NULL) && !(old_parent->is_root) && 
			!(old_parent->member_of_list)){
			new_parent = find(newtree, old_parent->key);
	}
	attach(new_parent, n);
	
	collect_and_dispose(gr);
	return newtree;
}
// returns a new js tree with node of key removed
void* removejs(const void* root, const char* key){
	if((key == NULL) || (root == NULL) ) 
		return NULL;
	
	CString* keycs = new_cstring_str(key);
	Node* newtree = clone_node_skip((Node*)root, keycs);
	free_cstring(keycs);

	return newtree;
}
// creates a js tree from c style string (char*)
void* newjs(char* contents){
	Garbage* gr = new_garbage();
	CString* contents_cs = w(gr, new_cstring_str(contents), GrCString);
	ParserReport *pr = w(gr, new_report(), GrParsingReport);
	TokensList *tkl = w(gr, tokenize(contents_cs), GrTokensList);
	Node* root = parse( tkl , pr);
	// CString* report = w(gr, report_cstring(pr), GrCString);
	// char* reportstr = w(gr, str(report), GrDefault);
	// printf("%s\n", reportstr);
	collect_and_dispose(gr);
	return root;
}
// creates a copy of node
void* clonejs(void* node){
	return (void*)clone_node((Node*)node);
}
// releases the memory allocated to root 
void  freejs(void *root){
	free_node ( (Node*)root );
}
// returns the value in node
void* valjs(void *node){
	Node* n = (Node*)node;
	if((n->content_type == NCT_STRING) ||
	(n->content_type == NCT_NUMBER)){
		return n->value;
	}
	return NULL;
}
// returns the item with index i in the list in node
void* itemjs(void *node, int i){
	return list_item((Node*)node, i);
}
// returns a c style string representation of js tree in root
char* strjs(void *root){
	Garbage *gr = new_garbage();
	char* contents = str((CString*)w(gr, node_cstring((Node*)root), GrCString));
	collect_and_dispose(gr);
	return contents;
}
// finds and returns a pointer to the node with given key, or null if not found
void* getjs(void *root, char *key){
	Garbage* gr = new_garbage();
	CString *keycs = w(gr, new_cstring_str(key), GrCString);
	Node *n = find((Node*)root, keycs);
	collect_and_dispose(gr);
	return n;
}
/*end of func defs*/