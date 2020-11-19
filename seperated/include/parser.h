#ifndef PARSER_H
#define PARSER_H

#include "cstring.h"
#include "lexer.h"

/*start of typedef*/
typedef enum val_types {NCT_STRING, NCT_NUMBER, NCT_TRUE, 
						NCT_FALSE , NCT_NULL  , NCT_BLOCK, 
						NCT_LIST} NodeContentType;
typedef enum errno {ERR_UNEXPECTED_EOF, ERR_UNEXPECTED_SYM, 
					ERR_NO_MATCHING_BRACKET, ERR_KEY_MISSING,
					ERR_UNKNOWN_SYM, ERR_BLOCK_MISSING} ErrNo;
typedef enum blocktype {BLOCK_LIST, BLOCK_OBJECT, BLOCK_NONE} BlockType;
typedef struct node Node;
typedef struct parser_report ParserReport;
typedef struct block Block;
/*end of typedef*/

/*start of struct defs*/
typedef struct parser_report{
	char success;
	CString* message;
} ParserReport;

typedef struct node{
	CString *key;
	NodeContentType content_type;
	void* value;
	char member_of_list;
	char is_root;
	Node *next_sibling;
} Node;

typedef struct block{
	unsigned int  num_children;
	char  is_list;
	Node *first_child; // list of children
	Node *last_child;
} Block;
/*end of struct defs*/

/*start of func protos*/
Node* list_item(Node* n, int i);
Node* find(Node* root, CString* key);
Node* find_parent(Node* root, CString* key);
Node* clone_node(const Node *n);
Node*  clone_node_skip(const Node* n, const CString* skipkey);
Node *parse(const TokensList *list, ParserReport *pr);
TokensList *unparse(const Node* node);
void attach(Node* parent, Node* child);

ParserReport *new_report();
void free_report(ParserReport *pr);
void free_node(Node *n);

CString* node_cstring(const Node *c);
CString* report_cstring(const ParserReport *pr);
/*end of func protos*/

#endif
