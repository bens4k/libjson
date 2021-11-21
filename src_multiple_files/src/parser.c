#include "../include/parser.h"
#include "../include/stdlibs.h"
#include "../include/stack.h"
#include "../include/garbage.h"

/*start of func defs*/
// returns a cstring representation of a node
CString* node_cstring(const Node *node){
	Garbage *gr = new_garbage();
	CString* cs = w(gr, new_cstring(0), GrCString);
	if(node != NULL){
		if(!(node->is_root) && !(node->member_of_list)){
			cs = w(gr, join(cs, 
					(CString*) w(gr, enclose(node->key, '\"', '\"'), GrCString)
				, 0), GrCString);
			cs = w(gr, join(cs, NULL, ':'), GrCString);
		}
		ulong num_size = 0;
		char* buff = NULL;
		switch(node->content_type){
			case NCT_STRING:
			{
				CString *enclosed = w(gr, enclose((CString*)node->value, '\"', '\"'), GrCString);
				cs = w(gr, join(cs, enclosed, ' '), GrCString);
				break;
			}
			case NCT_NUMBER:
			{
				CString* double_cs = w(gr, 
							new_cstring_flo(*((double*)node->value))
						, GrCString);
				cs = w(gr, join(cs, double_cs, ' '), GrCString);
				break;
			}
			case NCT_FALSE:
				cs = w(gr, joinstr(cs, "false", ' '), GrCString);
				break;
			case NCT_TRUE:
				cs = w(gr, joinstr(cs, "true", ' '), GrCString);
				break;
			case NCT_NULL:
				cs = w(gr, joinstr(cs, "null", ' '), GrCString);
				break;
			case NCT_BLOCK:
			{
				cs = w(gr, joinstr(cs, "{\n", 0), GrCString);
				Node *q = ((Block*)(node->value))->first_child ;
				while (q != NULL){
					CString* ncs = w(gr, node_cstring(q), GrCString);
					cs = w(gr, join(cs, ncs, 0), GrCString);
					q = q->next_sibling;
				}
				cs = w(gr, join(cs, NULL, '}'), GrCString);
				break;
			}
			case NCT_LIST:
			{
				cs = w(gr, joinstr(cs, "[\n", 0), GrCString);
				Node *q = ((Block*)(node->value))->first_child ;
				while (q != NULL){
					CString* ncs = w(gr, node_cstring(q), GrCString);
					cs = w(gr, join(cs, ncs, 0), GrCString);
					q = q->next_sibling;
				}
				cs = w(gr, joinstr(cs, "\n]", 0), GrCString);
				break;
			}
		}
		if(node->next_sibling != NULL)
			cs = w(gr, join(cs, NULL, ','), GrCString);
		if(!(node->member_of_list) && !(node->is_root))
			cs = w(gr, join(cs, NULL, '\n'), GrCString);
	}	
	CString *final_cs = csclone(cs);
	collect_and_dispose(gr);
	return final_cs;
}
// returns a cstring representation of a parser report
CString* report_cstring(const ParserReport *pr){
	if(pr != NULL){
		Garbage* gr = new_garbage();
		CString *cs = w(gr, new_cstring(0), GrCString);
		cs = w(gr, joinstr(cs, pr->success?"[SUCCESS]":"[FAILED]", 0), GrCString);
		if(!pr->success)
			cs = w(gr, join(cs, pr->message, ' '), GrCString);
		CString* final_cs = csclone(cs);
		collect_and_dispose(gr);
		return final_cs;
	}
	return NULL;
}
// creates a new parser report
ParserReport *new_report(){
	ParserReport *pr = (ParserReport*)malloc(sizeof(ParserReport));
	if (pr == NULL) return NULL;
	pr->success = 1;
	pr->message = NULL;
	return pr;
}
// creates a new block
Block* new_block(char is_lst){
	Block *b = (Block*)malloc(sizeof(Block));
	if (b == NULL) return NULL;
	b->is_list = is_lst;
	b->first_child = NULL;
	b->last_child = NULL;
	b->num_children = 0;
	return b;
}
// creates a new node of a given type and value
Node*  new_node(CString* key, NodeContentType vtype, void *value){
	Node * new_n = (Node*)malloc(sizeof(Node));
	if (new_n == NULL) return NULL;
	new_n->member_of_list = 0;
	new_n->key = key;
	new_n->content_type = vtype;
	new_n->value = value;
	new_n->is_root = 0;
	new_n->next_sibling = NULL;
	return new_n;
}
// returns a copy of a given node
Node*  clone_node(const Node *n){
	if(n != NULL){
		// create & copy data
		Node* new_n = new_node(
			csclone(n->key), 
			n->content_type, 
			NULL
		);
		new_n->is_root = n->is_root;
		new_n->member_of_list = n->member_of_list;
		// clone & assign values
		if(n->content_type == NCT_NUMBER){
			double *d = malloc(sizeof(double));
			if (d == NULL) return NULL;
			*d = *((double*)n->value);
			new_n->value = d;
		}else if (n->content_type == NCT_STRING){
			new_n->value = csclone((CString*)n->value);
		}else if((n->content_type == NCT_BLOCK) || (n->content_type == NCT_LIST)){
			// clones & assign children
			Block* old_blo = (Block*)n->value;
			Block* new_blo = new_block(old_blo->is_list);
			new_blo->num_children = old_blo->num_children;
			
			Node* current = old_blo->first_child;
			while(current != NULL){
				if(new_blo->last_child == NULL){ // empty block
					new_blo->first_child = clone_node(current);
					new_blo->last_child = new_blo->first_child;
				}else{ // attach to end
					new_blo->last_child->next_sibling = clone_node(current);
					new_blo->last_child = new_blo->last_child->next_sibling;
				}
				current = current->next_sibling;
			}
			new_n->value = new_blo;
		}
		return new_n;
	}
	return NULL;
}
// returns a copy of a given node without the node with given key
Node*  clone_node_skip(const Node* n, const CString* skipkey){
	if(n != NULL){
		if(!(n->is_root) && !(n->member_of_list)){
			if(equals(n->key, skipkey))
				return NULL;
		}
		// create & copy data
		Node* new_n = new_node(
			csclone(n->key), 
			n->content_type, 
			NULL
		);
		new_n->is_root = n->is_root;
		new_n->member_of_list = n->member_of_list;
		// clone & assign values
		if(n->content_type == NCT_NUMBER){
			double *d = malloc(sizeof(double));
			if (d == NULL) return NULL;
			*d = *((double*)n->value);
			new_n->value = d;
		}else if (n->content_type == NCT_STRING){
			new_n->value = csclone((CString*)n->value);
		}else if((n->content_type == NCT_BLOCK) || (n->content_type == NCT_LIST)){
			// clones & assign children
			Block* old_blo = (Block*)n->value;
			Block* new_blo = new_block(old_blo->is_list);
			new_blo->num_children = old_blo->num_children;
			
			Node* current = old_blo->first_child;
			while(current != NULL){
				Node* cloned_node = clone_node_skip(current, skipkey);
				if(cloned_node != NULL){
					if(new_blo->last_child == NULL){ // empty block
						new_blo->first_child = cloned_node;
						new_blo->last_child = new_blo->first_child;
					}else{ // attach to end
						new_blo->last_child->next_sibling = cloned_node;
						new_blo->last_child = new_blo->last_child->next_sibling;
					}
				}
				current = current->next_sibling;
			}
			new_n->value = new_blo;
		}
		return new_n;
	}
	return NULL;
}
// returns the item with index i in node list n
Node*  list_item(Node* n, int i){
	if(n->content_type == NCT_LIST){
		Node *p = ((Block*)n->value)->first_child;
		int index = 0;
		while(p != NULL){
			if(index == i)
				return p->value;
			p = p->next_sibling;
			index++;
		}
	}
	return NULL;
}
// finds and returns pointer to the node with given key, or null if not found
Node*  find(Node* root, CString* key){
	if(( !root->member_of_list ) && ( ! root->is_root)){
		if( equals(root->key, key) )
			return root;
	}
	if((root->content_type == NCT_BLOCK) || 
		(root->content_type == NCT_LIST)){
		Node *p = ((Block*)root->value)->first_child;
		while(p != NULL){
			Node *g = find(p, key);
			if(g != NULL)
				return g;
			p = p->next_sibling;
		}
	}
	return NULL;
}
// finds and returns pointer to the parent of node with given key, or null if not found
Node*  find_parent(Node* root, CString* key){
	if((root->content_type == NCT_BLOCK) || 
		(root->content_type == NCT_LIST)){
		Node *p = ((Block*)root->value)->first_child;
		while(p != NULL){
			if(!(p->member_of_list)){
				if(equals(p->key, key))
					return root;
			}
			Node* par = find_parent(p, key);
			if(par != NULL)
				return par;
			p = p->next_sibling;
		}
	}
	return NULL;
}

void   free_node(Node *n){
	if(n == NULL) return;
	// key
	if( !(n->is_root) && !(n->member_of_list))
		free_cstring(n->key);
	// contents
	if(n->content_type == NCT_STRING) //value string
		free_cstring((CString*)n->value);
	else if(n->content_type == NCT_NUMBER) // value number
		free((double*)n->value);
	else if((n->content_type == NCT_BLOCK) || (n->content_type == NCT_LIST)){ //children
		Block* blo = (Block*)n->value;
		Node *current_child = blo->first_child;
		while(current_child != NULL){
			Node* next = current_child->next_sibling;
			free_node(current_child);
			current_child = next;
		}
		free(blo);
	}
	// node itself
	free(n);
}

void   free_report(ParserReport *pr){
	if( pr->message != NULL)
		free_cstring(pr->message);
	free(pr);
}
// attach child node (at the end) to parent node if parent is a block type node
void attach(Node* parent, Node* child){
	if((parent->content_type == NCT_BLOCK) ||
		(parent->content_type == NCT_LIST)){
			Block* parent_blk = (Block*)parent->value;
			if(parent_blk->first_child == NULL){
				parent_blk->first_child = child;
			}else{
				parent_blk->last_child->next_sibling = child;
			}
			parent_blk->last_child = child;
	}
}
// returns the type of the top block (list or object) in stack s 
BlockType top_block_type(const Stack *s){
	Block *b = (Block*)top(s);
	return (b != NULL) ? (b->is_list? BLOCK_LIST : BLOCK_OBJECT) : BLOCK_NONE ;
}
// fill report (given as argument) and cleanup after a parsing error is encountered
Node *fail(Garbage* garbage, ParserReport *report, 
			ErrorNum error, const Token *current,const Token *next){
	if(garbage != NULL)
		collect_and_dispose(garbage);
	if(report != NULL){
		Garbage* gr = new_garbage();
		report->success = 0;
		CString* msg = NULL;
		switch (error){
			case ERR_UNKNOWN_SYM:
				msg = w(gr, new_cstring_str("Unrecognized symbol"), GrCString);
				if(current != NULL){
					CString* enclosed_tkn = w(gr, token_cstring(current), GrCString);
					enclosed_tkn = w(gr, enclose(enclosed_tkn, '\"', '\"'),GrCString);
					msg = w(gr, join(msg, enclosed_tkn, ' '), GrCString);
					msg = w(gr, joinstr(msg, "Line: ", ' '), GrCString);
					msg = w(gr, join(msg, w(gr, new_cstring_dec(current->line_num), GrCString), 0), GrCString);
				}
				break;
			case ERR_BLOCK_MISSING:
				msg = w(gr, new_cstring_str("Missing object or array"), GrCString);
				if(current != NULL){
					msg = w(gr, joinstr(msg, "for", ' '), GrCString);
					msg = w(gr, join(msg, token_cstring(current), ' '), GrCString);
					msg = w(gr, joinstr(msg, "Line: ", ' '), GrCString);
					msg = w(gr, join(msg, w(gr, new_cstring_dec(current->line_num), GrCString), 0), GrCString);
				}
				break;
			case ERR_UNEXPECTED_EOF:
				msg = w(gr, new_cstring_str("Unexpected end of file after"), GrCString);
				if(current != NULL)
					msg = w(gr, join(msg, token_cstring(current), ' '), GrCString);
				break;
			case ERR_UNEXPECTED_SYM:
				msg = w(gr, new_cstring_str("Unexpected symbol"), GrCString);
				if(next != NULL)
					msg = w(gr, join(msg, w(gr, enclose(token_cstring(next), '\"', '\"'), GrCString), ' '), GrCString);
				if(current != NULL){
					msg = w(gr, joinstr(msg, "after", ' '), GrCString);
					msg = w(gr, join(msg, token_cstring(current), ' '), GrCString);
				}
				if(next != NULL){
					msg = w(gr, joinstr(msg, "at Line: ", ' '), GrCString);
					msg = w(gr, join(msg, w(gr, new_cstring_dec(next->line_num), GrCString), 0), GrCString);
				}
				break;
			case ERR_NO_MATCHING_BRACKET:
				msg = w(gr, new_cstring_str("No Matching bracket for "), GrCString);
				if(current != NULL){
					msg = w(gr, join(msg, w(gr, token_cstring(current), GrCString), ' '), GrCString);
					msg = w(gr, joinstr(msg, "Line: ", ' '), GrCString);
					msg = w(gr, join(msg, w(gr, new_cstring_dec(current->line_num), GrCString), 0), GrCString);
				}
				break;
			case ERR_KEY_MISSING:
				msg = w(gr, new_cstring_str("Key missing"), GrCString);
				if(current != NULL){
					msg = w(gr, joinstr(msg, "Line: ", ' '), GrCString);
					msg = w(gr, join(msg, w(gr, new_cstring_dec(current->line_num), GrCString), 0), GrCString);
				}
				break;
			default:
				msg = w(gr, new_cstring_str("Unkown error!"), GrCString);
				break;
		}
		report->message = msg!=NULL? csclone(msg): NULL;
		collect_and_dispose(gr);
	}
	return NULL;
}
// link node to top block (most recent block) found in stack s 
char link_node(const Stack *s, Node *node){
	Block *b = (Block*)top(s);
	if(b != NULL){
		if(b->first_child == NULL){
			b->first_child = node;
			b->last_child = node;
		}else{
			b->last_child->next_sibling = node;
			b->last_child = node;
		}
		b->num_children++;
		return 0;
	}
	return 1;
}
// parse tokens list into a js tree
Node *parse(const TokensList *list, ParserReport *pr){
	Garbage* gr = new_garbage();
	Token *t = list->head;
	Node *root = NULL;
	Stack *block_stack = w(gr, new_stack(), GrStack);
	char init_state = 1;
	CString *last_key = NULL;
	// checking for root block
	if(t == NULL)
		return fail(gr, pr, ERR_BLOCK_MISSING, t, NULL);
	if((t->type != TKN_LBRACE) && (t->type != TKN_LBRACKET))
		return fail(gr, pr, ERR_BLOCK_MISSING, t, NULL);
	
	while(t != NULL){
		if(t->type == TKN_UNKNOWN)
			return fail(gr, pr, ERR_UNKNOWN_SYM, t, t->next);
		if(t->next != NULL)
			if(t->next->type == TKN_UNKNOWN)
				return fail(gr, pr, ERR_UNKNOWN_SYM, t->next, NULL);
		switch(t->type){
			case TKN_LBRACE:
			{
				Block *object_block = new_block(0);
				if( init_state ){ // came from initial state i.e root block
					root = w(gr, new_node(NULL, NCT_BLOCK, (void*)object_block), GrNode);
					root->is_root = 1;
					init_state = 0;
				}else{ // came after a string and colon i.e inner block
					switch( top_block_type(block_stack)){
						case BLOCK_LIST:
						{
							Node *chi = new_node(NULL, NCT_BLOCK, (void*)object_block);
							chi->member_of_list = 1;
							link_node(block_stack, chi);
							break;
						}
						case BLOCK_OBJECT:
						{
							if(last_key == NULL)
								return fail(gr, pr, ERR_UNEXPECTED_SYM, t, t->next);
							Node *chi = new_node(csclone(last_key), NCT_BLOCK, (void*)object_block);
							if( link_node(block_stack, chi) )
								return fail(gr, pr, ERR_UNEXPECTED_SYM, t, t->next);
							break;
						}
						default:
							return fail(gr, pr, ERR_BLOCK_MISSING, t, NULL);
					}
				}
				push(block_stack, (void*)object_block);
 
 				if(t->next != NULL){
					if((t->next->type != TKN_STRING) && (t->next->type != TKN_RBRACE))
						return fail(gr, pr, ERR_KEY_MISSING, t, NULL);
				}else
					return fail(gr, pr, ERR_UNEXPECTED_EOF, t, NULL);
				break;
			}
			case TKN_LBRACKET:
			{
				Block *array_block = new_block(1);
				if( init_state ){ // came from initial state i.e root list
					root = w(gr, new_node(NULL, NCT_LIST, (void*)array_block), GrNode);
					root->is_root = 1;
					init_state = 0;
				}else{ // came after a string and colon i.e inner list
					if(top_block_type(block_stack) == BLOCK_LIST){
						Node *chi = new_node(NULL, NCT_LIST, (void*)array_block);
						chi->member_of_list = 1;
						link_node(block_stack, chi);
					}else if(top_block_type(block_stack) == BLOCK_OBJECT){
						if(last_key == NULL) 
							return fail(gr, pr, ERR_KEY_MISSING, t, t->next);
						Node *chi = new_node(csclone(last_key), NCT_LIST, (void*)array_block);
						if( link_node(block_stack, chi) )
							return fail(gr, pr, ERR_BLOCK_MISSING, t, t->next);
					}
				}
				push(block_stack, (void*)array_block);

				if(t->next != NULL){
					if(	(t->next->type == TKN_COLON) || 
						(t->next->type == TKN_COMMA) ||
						(t->next->type == TKN_RBRACE) ){
							return fail(gr, pr, ERR_UNEXPECTED_SYM, t, t->next);
					}
				}else
					return fail(gr, pr, ERR_UNEXPECTED_EOF, t, NULL);
				break;
			}
			case TKN_RBRACE:
			{
				if(top_block_type(block_stack) == BLOCK_NONE) // single closing }
					return fail(gr, pr, ERR_BLOCK_MISSING, t, NULL);
				last_key = NULL;
				pop(block_stack);
				
				if( t->next != NULL){
					if(top_block_type(block_stack) == BLOCK_LIST){
						if((t->next->type != TKN_COMMA) 
						   && (t->next->type != TKN_RBRACKET)){
							return fail(gr, pr, ERR_UNEXPECTED_SYM, t, t->next);
						}
					}else if( top_block_type(block_stack) == BLOCK_OBJECT ){
						if((t->next->type != TKN_COMMA) && (t->next->type != TKN_RBRACE))
							return fail(gr, pr, ERR_UNEXPECTED_SYM, t, t->next);
					}
				}else if(top_block_type(block_stack) != BLOCK_NONE)
					return fail(gr, pr, ERR_UNEXPECTED_EOF, t, NULL);
				break;
			}
			case TKN_RBRACKET:
				if(top_block_type(block_stack) == BLOCK_LIST)
					pop(block_stack);

				if( t->next != NULL){
					switch(top_block_type(block_stack)){
						case BLOCK_LIST:
							if((t->next->type != TKN_COMMA) 
								&& (t->next->type != TKN_RBRACKET))
								return fail(gr, pr, ERR_UNEXPECTED_SYM, t, t->next);
							break;
						case BLOCK_OBJECT:
							if((t->next->type != TKN_COMMA) && (t->next->type != TKN_RBRACE)){
								return fail(gr, pr, ERR_UNEXPECTED_SYM, t, t->next);
							}
							break;
						default:
							return fail(gr, pr, ERR_BLOCK_MISSING, t->next, NULL);					
					}
				}else if(top_block_type(block_stack) != BLOCK_NONE)
						return fail(gr, pr, ERR_UNEXPECTED_EOF, t, NULL);
				break;
			case TKN_STRING:
			{
				if(t->next == NULL)
					return fail(gr, pr, ERR_UNEXPECTED_EOF, t, NULL);
				
				switch(top_block_type(block_stack)){
					case BLOCK_LIST:
					{
						Node *chi = new_node(NULL, NCT_STRING, (void*)csclone((CString*)t->value));
						chi->member_of_list = 1;
						link_node(block_stack, chi);
						if((t->next->type != TKN_COMMA) && (t->next->type != TKN_RBRACKET))
							return fail(gr, pr, ERR_UNEXPECTED_SYM, t, t->next);
						break;
					}
					case BLOCK_OBJECT:
						if(t->next->type == TKN_COLON){
							last_key = (CString*)t->value;
							if((t->value == NULL) || (((CString*)t->value)->size == 0))
								return fail(gr, pr, ERR_KEY_MISSING, t, t->next);
						}else 
							return fail(gr, pr, ERR_UNEXPECTED_SYM, t, t->next);
						break;
					default: 
						return fail(gr, pr, ERR_BLOCK_MISSING, t, t->next);
				}
				break;
			}
			case TKN_COLON:
			{
				if(t->next == NULL)
					return fail(gr, pr, ERR_UNEXPECTED_EOF, t, NULL);
				
				Token *q = t->next;
				if((q->type == TKN_STRING) || (q->type == TKN_NUMBER) ||
				   (q->type == TKN_TRUE)   || (q->type == TKN_FALSE)  || 
				   (q->type == TKN_NULL)){
					
					if(last_key == NULL)
						return fail(gr, pr, ERR_KEY_MISSING, q, q->next);
					Node *chi = NULL;
					switch(q->type){
						case TKN_STRING:
							chi = new_node(csclone(last_key), NCT_STRING,
											(void*)csclone((CString*)q->value) );
							break;
						case TKN_NUMBER:
						{
							double *dd = (double*)malloc(sizeof(double));
							if (dd != NULL) {
								*dd = *((double*)q->value);
								chi = new_node(csclone(last_key), NCT_NUMBER, (void*)dd);
							}
							break;
						}
						case TKN_TRUE:
							chi = new_node(csclone(last_key), NCT_TRUE, NULL);
							break;
						case TKN_FALSE:
							chi = new_node(csclone(last_key), NCT_FALSE, NULL);
							break;
						case TKN_NULL:
							chi = new_node(csclone(last_key), NCT_NULL, NULL);
							break;
					}
					if( link_node(block_stack, chi) )
						return fail(gr, pr, ERR_BLOCK_MISSING, q, q->next);
					t = t->next;
					if(t->next != NULL){
						if((t->next->type != TKN_COMMA) && (t->next->type != TKN_RBRACE))
							return fail(gr, pr,	ERR_UNEXPECTED_SYM, q, q->next);
					}else 
						return fail(gr, pr, ERR_UNEXPECTED_EOF, q, NULL);
				}else if((q->type != TKN_LBRACE) && (q->type != TKN_LBRACKET)){
					return fail(gr, pr, ERR_UNEXPECTED_SYM, q, q->next);
				}
				break;
			}
			case TKN_COMMA:
			{
				if(top_block_type(block_stack) == BLOCK_NONE)
					return fail(gr, pr, ERR_BLOCK_MISSING, t, NULL);
				
				if(t->next != NULL){
					if(top_block_type(block_stack) == BLOCK_LIST){
						if((t->next->type == TKN_RBRACE) || (t->next->type == TKN_RBRACKET)
							|| (t->next->type == TKN_COLON) || (t->next->type == TKN_COMMA))
							return fail(gr, pr, ERR_UNEXPECTED_SYM, t, t->next);
					}else if(top_block_type(block_stack) == BLOCK_OBJECT){
						if(t->next->type != TKN_STRING)
							return fail(gr, pr,	ERR_KEY_MISSING, t, t->next);
					}
				}else
					return fail(gr, pr, ERR_UNEXPECTED_EOF, t, NULL);
				
				last_key = NULL;
				break;
			}
			default:
			{
				if(top_block_type(block_stack) == BLOCK_LIST){
					Node *chi = NULL;
					switch (t->type){
						case TKN_NUMBER:
						{
							double *dd = (double*)malloc(sizeof(double));
							if (dd != NULL) {
								*dd = *((double*)t->value);
								chi = new_node(NULL, NCT_NUMBER, (void*)dd);
							}
							break;	
						}
						case TKN_TRUE:
							chi = new_node(NULL, NCT_TRUE, NULL);
							break;
						case TKN_FALSE:
							chi = new_node(NULL, NCT_FALSE, NULL);
							break;
						case TKN_NULL:
							chi = new_node(NULL, NCT_NULL, NULL);
							break;
					}
					if(chi != NULL){
						chi->member_of_list = 1;
						link_node(block_stack, chi);
					}
					if(t->next == NULL)
						return fail(gr, pr, ERR_UNEXPECTED_EOF, t, NULL);
					if((t->next->type != TKN_COMMA) && (t->next->type != TKN_RBRACKET))
						return fail(gr, pr, ERR_UNEXPECTED_SYM, t, t->next);
				}
			}
		}
		t = t->next;
	}
	Node* final_root = clone_node(root);
	collect_and_dispose(gr);
	return final_root;
}
// returns a tokens list created from a given js tree
TokensList *unparse(const Node* node){
	if(node == NULL) return NULL;
	TokensList *list = new_tokens_list();
	if((!node->is_root) && (!node->member_of_list)){
		Token *tkey = new_token(TKN_STRING, (void*)csclone(node->key), 0);
		Token *tcolon = new_token(TKN_COLON, NULL, 0);
		add_token_to_list(list, tkey);
		add_token_to_list(list, tcolon);
	}
	switch(node->content_type){
		case NCT_BLOCK:
		{
			Token *lb = new_token(TKN_LBRACE,  NULL, 0);
			add_token_to_list(list, lb);
			concat_token_lists(list, unparse(((Block*)node->value)->first_child)) ;
			Token *rb = new_token(TKN_RBRACE, NULL, 0);
			add_token_to_list(list, rb);
			break;
		}
		case NCT_LIST:
		{
			Token *lb = new_token(TKN_LBRACKET,  NULL, 0);
			add_token_to_list(list, lb);
			concat_token_lists(list, unparse(((Block*)node->value)->first_child)) ;
			Token *rb = new_token(TKN_RBRACKET, NULL, 0);
			add_token_to_list(list, rb);
			break;
		}
		case NCT_TRUE:
			add_token_to_list(list, new_token(TKN_TRUE, NULL, 0));
			break;
		case NCT_FALSE:
			add_token_to_list(list, new_token(TKN_FALSE, NULL, 0));
			break;
		case NCT_STRING:
		{
			Token *ts = new_token(TKN_STRING, (void*)csclone((CString*)node->value), 0);
			add_token_to_list(list, ts);
			break;
		}
		case NCT_NULL:
			add_token_to_list(list, new_token(TKN_NULL, NULL, 0));
			break;
		case NCT_NUMBER:
		{
			double *dd = (double*)malloc(sizeof(double));
			if (dd != NULL) {
				*dd = *((double*)node->value);
				add_token_to_list(list, new_token(TKN_NUMBER, dd, 0));
			}
			break;
		}
	}
	return list;
}
/*end of func defs*/