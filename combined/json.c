#include "json.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef unsigned long ulong;
typedef unsigned int uint;

typedef struct cstring CString;
typedef struct splits SplitsList;

typedef enum {GrNode, GrCString, GrStack, GrTokensList, GrSplitsList,
                GrParsingReport, GrDefault} GrType;
typedef struct garbage Garbage;
typedef struct garbage_entry GarbageEntry;

typedef enum { TKN_STRING, TKN_NUMBER, TKN_TRUE, TKN_FALSE, TKN_NULL, TKN_LBRACE, TKN_RBRACE, TKN_COLON, TKN_COMMA,
				TKN_LBRACKET, TKN_RBRACKET, TKN_UNKNOWN} TokenType;
typedef struct token Token;
typedef struct tokens_list TokensList;

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

typedef struct stack_node StackNode;
typedef struct stack Stack;

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct stack {
	StackNode *top;
	StackNode *head;
} Stack;

typedef struct cstring{
	char* data;
	unsigned long size;
} CString;

typedef struct splits{
	unsigned int size;
	CString** splits;
} SplitsList;

typedef struct garbage {
    Stack* stack;
} Garbage;

typedef struct garbage_entry {
    void *object;
    GrType type;
} GarbageEntry;

typedef struct token{
	void *value;
	ulong line_num;
	TokenType type;
	Token *next;
} Token;

typedef struct tokens_list{
	Token *head;
	Token *tail;
	uint size;
} TokensList;

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

typedef struct stack_node{
	void *payload;
	StackNode *next;
} StackNode;

Stack *new_stack();
void push(Stack *s, void *p);
void *pop(Stack *s);
void *top(const Stack *s);
void free_stack(Stack *s);
char is_stack_empty(const Stack *s);

char lower_case(const char c);
char is_digit(const char c);
char is_non_zero_digit(const char c);
char is_sign(const char c);
char part_of_number(const char c);
char start_of_number(const char c);
char char_to_digit(const char c);
char digit_to_char(const char d);

CString *join(const CString *cs1, const CString *cs2, char delimiter);
CString *joinstr(const CString *cs, char* s, char delimiter);
CString *enclose(const CString *cs, char c1, char c2);
CString *lowercase(const CString *cs);
CString *unescape_special_chars(const CString *src_cs);
CString *sub_cstring(const CString *cs, long start, long end);
CString *new_cstring_flo(double fl);
CString *new_cstring_dec(int dec);
CString *new_cstring_char(char c);
CString *new_cstring_str(const char* s_str);
CString *new_cstring(unsigned long size);
void     free_cstring(CString *cs);

char   *str(const CString *cs);
double  todouble(const CString *cs);
char    equals(const CString *cs1, const CString *cs2);

unsigned long subcopy(CString *dest, const CString *src, 
	unsigned long dest_start, unsigned long src_start, unsigned long amount);
CString  *csclone(const CString *cs);
CString  *strip_sequnce(const CString* cs, const CString* seq);
CString  *strip_char(const CString* cs, char c);

SplitsList *new_splits_list(unsigned int c);
SplitsList *split(const CString *cs, char c);
CString    *join_splits(const SplitsList *list, char join_c);
void        free_splits_list(SplitsList *list);

Garbage *new_garbage();
void* w(const Garbage *gr, void* ptr, GrType type); // watch object for collection
unsigned int collect(const Garbage *gr);
void dispose(Garbage *gr);
void collect_and_dispose(Garbage *gr);

TokensList *tokenize(const CString *cs);
CString *untokenize(const TokensList *list);
TokensList *new_tokens_list();
Token *new_token(TokenType ty, void *val, ulong line);
void add_token_to_list(TokensList *list, Token *t);
void free_tokens_list(TokensList *list);
void concat_token_lists(TokensList *l1, TokensList *l2);
CString* token_cstring(const Token *t);
CString* tokens_list_cstring(const TokensList *list);

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

char lower_case(const char c){
	return ( (c >= 65 ) && (c <= 90)  ) ? c+32 : c; 
}
char is_digit(const char c){
	return ( (c >= 48 ) && (c <= 57)  ) ? 1 : 0 ;
}
char is_non_zero_digit(const char c){
	return is_digit(c) && c != '0' ;
}
char is_sign(const char c){
	return ((c == '-') || (c == '+')) ? 1 : 0;
}
char part_of_number(const char c){
	return (is_digit(c) || is_sign(c) || (lower_case(c) == 'e') || (c == '.')) ? 1 : 0;
}
char start_of_number(const char c){
	return (is_digit(c) || is_sign(c)) ? 1 : 0;
}
// '9' to 9
char char_to_digit(const char c){
	return c - 48;
}
// 9 to '9'
char digit_to_char(const char d){
	if((d <= 9) && (d >= 0)){
		return d + 48;
	}
	return 0;
}

CString *new_cstring(ulong size){
	CString *cs = (CString*)malloc(sizeof(CString));
	cs->size = size;
	cs->data = (size == 0)? NULL : (char*)malloc(cs->size);
	return cs;
}
CString *new_cstring_str(const char* s){
	uint size = strlen(s);
	CString *cs = new_cstring(size);
	// for(int i=0; i<cs->size ; i++)
	// 	cs->data[i] = s[i];
	memcpy(cs->data, s, size);
	return cs;
}
CString *new_cstring_char(char c){
	CString* cs = new_cstring(1);
	cs->data[0] = c;
	return cs;
}
CString *new_cstring_dec(int dec){
	uint cur_b = 1;
	int length = 0;
	char sign = (dec >= 0) ? 0 : 1;
	int a = sign? dec * (-1) : dec;
	if(sign) length++;
	while((int)(a / cur_b) > 0 ){
		cur_b *= 10;
		length++;
	}
	cur_b = 10;
	uint prev_b = 1;
	int end_index = sign ? 1 : 0 ;
	CString *int_ch = new_cstring(length);
	for(int i = length-1;  i >= end_index ; i--){
		int k = ((a%cur_b - a%prev_b) / prev_b );
		int_ch->data[i] =  digit_to_char( (int)k );
		prev_b = cur_b;
		cur_b *= 10;
	}
	if(sign) int_ch->data[0] = '-';
	return int_ch;
}
CString *new_cstring_flo(double fl){
	ulong size = snprintf(NULL, 0, "%lf", fl);
	char* buff = malloc(size+1);
	sprintf(buff, "%lf", fl);
	CString* dounle_ch = new_cstring_str(buff);
	free(buff);
	return dounle_ch;
}
void     free_cstring(CString *cs){
	if(cs->size > 0)
		free(cs->data);
	free(cs);
}

static void uncover_escaped_quotes(CString *cs){
	for(uint i=0; i< cs->size ; i++){
		if(cs->data[i] == '\\'){
			if(((i+1) < cs->size) && (cs->data[i+1] == 'q')){
				cs->data[i+1] = '"';
			}
		}
	}
}
static void cover_escaped_quotes(CString *cs){
	for(uint i=0; i< cs->size ; i++){
		if(cs->data[i] == '\\'){
			if(((i+1) < cs->size) && (cs->data[i+1] == '"')){
				cs->data[i+1] = 'q';
			}
		}
	}
}

ulong subcopy(CString *dest, const CString *src, ulong dest_start, 
			ulong src_start, ulong amount){
	ulong amount_copied = 0;
	// ensure starts are in bounds of chunks
	if((dest_start >= 0) && (dest_start < dest->size) 
	&& (src_start >= 0) && (src_start < src->size)){
		ulong i = dest_start, j = src_start;
		while((i < dest->size) && (j < src->size) && (j < src_start+amount)){
			dest->data[i++] = src->data[j++];
			amount_copied++;
		}
	}
	return amount_copied;
}
char equals(const CString *cs1, const CString *cs2){
	if((cs1 == NULL) || (cs2 == NULL))
		return 0;
	if( cs1->size == cs2->size){
		for(uint i=0; i< cs1->size ; i++){
			if(cs1->data[i] != cs2->data[i])
				return 0;
		}
		return 1;
	}
	return 0;
}

double todouble(const CString *cs){
	char* strd = str(cs);
	double v = strtod(strd, NULL);
	free(strd);
	return v;
}
char*  str(const CString *cs){
	char *s = NULL;
	if(cs->size > 0){
		s = (char*)malloc(cs->size + 1);
		for(uint i=0; i< cs->size ; i++)
			s[i] = cs->data[i];
		s[cs->size] = '\0';
	}
	return s;
}

SplitsList* new_splits_list(uint size){
	SplitsList* list = (SplitsList*)malloc(sizeof(SplitsList));
	list->size = size;
	list->splits = (CString**)malloc((list->size)*sizeof(CString*));
	return list;
}
SplitsList* split(const CString *cs, char c){
	uint count = 1; // count always 1 or greater
	for(ulong i=0; i< cs->size; i++){	// calc number of sub-chunks
		if(cs->data[i] == c)
			count++;
	}
	SplitsList *list = new_splits_list(count);
	// splitting
	ulong splits_cursor = 0;
	uint k = 0;
	if(list->size > 1){
		for(ulong i=0; i< cs->size; i++){
			if(cs->data[i] == c){
				CString *subcs = sub_cstring(cs, splits_cursor, i);
				list->splits[k++] = subcs;
				splits_cursor = i+1;
			}
		}
	}
	//adding last subchunk
	CString *subcs = sub_cstring(cs, splits_cursor, cs->size);
	list->splits[k++] = subcs;
	return list;
}
void 		free_splits_list(SplitsList *list){
	for(uint i=0; i< list->size; i++){
		free_cstring(list->splits[i]);
	}
	free(list->splits);
	free(list);
}

CString *csclone(const CString *cs){
	CString *ncs = NULL;
	if(cs != NULL){
		ncs = new_cstring(cs->size);
		for(uint i=0; i< cs->size; i++)
			ncs->data[i] = cs->data[i];
	}
	return ncs;
}
CString *sub_cstring(const CString *cs, long start, long end){
	// returns sub-cstring, start-inclusive, end-exclusive
	// interval constraints
	ulong e_start = (start < 0)? 0 : (start >= cs->size)? cs->size-1 : start;
	ulong e_end = (end < 0)? 0 : (end > cs->size)? cs->size : end;
	// overlapping constraints
	e_start = (e_start > e_end)? e_end : e_start;
	e_end = (e_end < e_start)? e_start : e_end;

	CString* ncs;
	if(e_end == e_start){
		ncs = new_cstring(0);
	}else{
		ncs = new_cstring(e_end - e_start);
		ulong j = 0;
		for(ulong i=e_start; i<e_end; i++){
			ncs->data[j++] = cs->data[i];
		}
	}
	return ncs;
}
CString *strip_char(const CString* cs, char c){
	CString *stripped_cs;
	uint count = 0;
	for(uint i = 0; i< cs->size ; i++){
		if(cs->data[i] == c)
			count++;
	}
	if( count > 0){
		stripped_cs = new_cstring(cs->size - count);
		uint j = 0;
		for(uint i = 0; i< cs->size ; i++){
			if(cs->data[i] != c)
				stripped_cs->data[j++] = cs->data[i];
		}
	}else 
		stripped_cs = csclone(cs);
	return stripped_cs;
}
CString *strip_sequence(const CString* cs, const CString* seq){
	CString * marked_cs = csclone(cs);
	for(ulong i=0; i< (cs->size); i++){
		if(cs->data[i] == seq->data[0]){
			char match = 1;
			if((i+(seq->size)) > cs->size){
				match = 0;
			}else{
				for(int k = 0; k < (seq->size); k++){
					if(cs->data[i+k] != seq->data[k]){
						match = 0;
						break;
					}
				}
			}
			if(match){ // mark
				for(uint j=i; j< i+(seq->size); j++)
					marked_cs->data[j] = '\0';
			}
		}
	}
	CString *stripped_cs = strip_char(marked_cs, '\0');
	free_cstring(marked_cs);
	return stripped_cs;
}
CString *unescape_special_chars(const CString *src_cs){
	if(src_cs == NULL)
		return NULL;
	// count how many chars to unescape
	uint count = 0;
	for(uint i=0; i < src_cs->size; i++){
		if(src_cs->data[i] == '\\') {
			if(i+1 < src_cs->size){
				char c = src_cs->data[i+1];
				if((c == 'r') || (c == '\\') || (c == 'n') || (c == '"')
					|| (c == 't') || (c == 'f') || (c == 'b') || (c == '/')){
					count++;
				}
			}
			i++;
		}
	}
	if(count > 0){
		CString *esc_ch = new_cstring(src_cs->size - count);
		uint j = 0, i = 0 ;
		while(i < src_cs->size){
			if(src_cs->data[i] == '\\'){
				if(i+1 < src_cs->size){
					switch(src_cs->data[i+1]){
						case 'n':
							esc_ch->data[j++] = '\n';
							break;
						case '\\':
							esc_ch->data[j++] = '\\';
							break;
						case 'r':
							esc_ch->data[j++] = '\r';
							break;
						case '"':
							esc_ch->data[j++] = '"';
							break;
						case 't':
							esc_ch->data[j++] = '\t';
							break;
						case 'f':
							esc_ch->data[j++] = '\f';
							break;
						case 'b':
							esc_ch->data[j++] = '\b';
							break;
						case '/':
							esc_ch->data[j++] = '/';
							break;
						default:
							esc_ch->data[j++] = src_cs->data[i++];
							continue;
							break;
					}
				}
				i++;
			}else esc_ch->data[j++] = src_cs->data[i];
			i++;
		}
		return esc_ch;
	}
	return csclone(src_cs);
}
CString *lowercase(const CString *cs){
	CString *lowercase_cs = csclone(cs);
	for(uint i=0; i< cs->size; i++)
		lowercase_cs->data[i] = lower_case(cs->data[i]);
	return lowercase_cs;
}
CString *enclose(const CString *cs, char c1, char c2){
	if(cs != NULL){
		CString *enclosed = new_cstring(cs->size + 2);
		enclosed->data[0] = c1;
		for(int i=0; i<cs->size; i++)
			enclosed->data[i+1] = cs->data[i];
		enclosed->data[enclosed->size - 1] = c2;
		return enclosed;
	}
	return NULL;
}
CString *joinstr(const CString *cs, char* s, char delimiter){
	CString* cs_str = new_cstring_str(s);
	CString* joinedcs = join(cs, cs_str, delimiter);
	free_cstring(cs_str);
	return joinedcs;
}
CString *join(const CString *cs1, const CString *cs2, char delimiter){
	CString *joined = NULL;
	ulong size1 = (cs1 != NULL) ? cs1->size : 0;
	ulong size2 = (cs2 != NULL) ? cs2->size : 0;
	joined = new_cstring(size1 + size2 + 1*(delimiter != 0));
	if(size1 > 0)
		for(int i=0; i<size1; i++)
			joined->data[i] = cs1->data[i];
	
	if(delimiter != 0)
			joined->data[size1] = delimiter;
	
	if(size2 > 0)
		for(int i=0; i<size2; i++)
			joined->data[size1 + 1*(delimiter != 0) + i] = cs2->data[i];
	
	return joined;
}
CString *join_splits(const SplitsList *list, char delimiter){
	// calculate total size, and allocate space
	ulong total_size = 0;
	for(uint i=0; i< list->size; i++)
		total_size += list->splits[i]->size;
	if(delimiter != 0)
		total_size += list->size - 1;
	CString *j_cs = new_cstring(total_size);
	ulong k = 0;
	for(uint i=0; i< list->size; i++){
		for(uint j=0; j< list->splits[i]->size; j++)
			j_cs->data[k++] = list->splits[i]->data[j];
		if((delimiter != 0) && (i < list->size - 1))
			j_cs->data[k++] = delimiter;
	}
	return j_cs;
}

Garbage *new_garbage(){
    Garbage *gr = malloc(sizeof(Garbage));
    gr->stack = new_stack();
    return gr;
}
unsigned int collect(const Garbage *gr){
    if(gr != NULL){
        while(!is_stack_empty(gr->stack)){
            GarbageEntry *ge = (GarbageEntry*) pop(gr->stack);
            if(ge->object != NULL){
                switch(ge->type){
                    case GrNode:
                        free_node((Node*)ge->object);
                        break;
                    case GrCString:
                        free_cstring((CString*)ge->object);
                        break;
                    case GrStack:
                        free_stack((Stack*)ge->object);
                        break;
                    case GrTokensList:
                        free_tokens_list((TokensList*)ge->object);
                        break;
                    case GrSplitsList:
                        free_splits_list((SplitsList*)ge->object);
                        break;
                    case GrParsingReport:
                        free_report((ParserReport*)ge->object);
                        break;
                    default:
                        free(ge->object);
                }
                free(ge);
            }
        }
    }
}
void dispose(Garbage *gr){
    free_stack(gr->stack);
    free(gr);
}
void collect_and_dispose(Garbage *gr){
    collect(gr);
    dispose(gr);
}
GarbageEntry *new_gr_entry(void* obj, GrType t){
    GarbageEntry* ge = malloc(sizeof(GarbageEntry));
    ge->object = obj;
    ge->type = t;
    return ge;
}
void* w(const Garbage *gr, void* ptr, GrType type){
    if(gr != NULL)
        push(gr->stack, new_gr_entry(ptr, type));
    return ptr;
}

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
void* removejs(const void* root, const char* key){
	if((key == NULL) || (root == NULL) ) 
		return NULL;
	
	CString* keycs = new_cstring_str(key);
	Node* newtree = clone_node_skip((Node*)root, keycs);
	free_cstring(keycs);

	return newtree;
}
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
void* clonejs(void* node){
	return (void*)clone_node((Node*)node);
}
void  freejs(void *root){
	free_node ( (Node*)root );
}
void* valjs(void *node){
	Node* n = (Node*)node;
	if((n->content_type == NCT_STRING) ||
	(n->content_type == NCT_NUMBER)){
		return n->value;
	}
	return NULL;
}
void* itemjs(void *node, int i){
	return list_item((Node*)node, i);
}
char* strjs(void *root){
	Garbage *gr = new_garbage();
	char* contents = str((CString*)w(gr, node_cstring((Node*)root), GrCString));
	collect_and_dispose(gr);
	return contents;
}
void* getjs(void *root, char *key){
	Garbage* gr = new_garbage();
	CString *keycs = w(gr, new_cstring_str(key), GrCString);
	Node *n = find((Node*)root, keycs);
	collect_and_dispose(gr);
	return n;
}

char* token_str[] = {"String", "Number", "true", "false", "null", "{", "}", ":", ",", "[", "]", "Unknown"};

CString* tokens_list_cstring(const TokensList *list){
	Garbage* gr = new_garbage();
	Token *t = list->head;
	CString* cs = w(gr, new_cstring(0), GrCString);
	while(t != NULL){
		CString* tcs = w(gr, token_cstring(t), GrCString);
		cs = w(gr, join(cs, tcs, ' '), GrCString);
		t = t->next;
	}
	CString* final_cs = csclone(cs);
	collect_and_dispose(gr);
	return final_cs;
}
CString* token_cstring(const Token *t){
	Garbage* gr = new_garbage();
	if(t != NULL){
		CString* cs = w(gr, new_cstring(0), GrCString);
		switch(t->type){
			case TKN_UNKNOWN:
				cs = w(gr, join(cs, (CString*)(t->value), 0), GrCString);
				break;
			case TKN_STRING:
				cs = w(gr, join(cs, (CString*)(t->value), 0), GrCString);
				cs = w(gr, enclose(cs, '\"', '\"'), GrCString);
				break;
			case TKN_NUMBER:
			{
				double d = *((double*)(t->value));
				CString* d_cs = w(gr, new_cstring_flo(d), GrCString);
				cs = w(gr, join(cs, d_cs, 0), GrCString);
				break;
			}
		}
		if((t->type != TKN_STRING) && (t->type != TKN_NUMBER) && (t->type != TKN_UNKNOWN))
			cs = w(gr, joinstr(cs, token_str[t->type], 0), GrCString);
		CString* final_cs = csclone(cs);
		collect_and_dispose(gr);
		return final_cs;
	}
	return NULL;
}
char reserved_char(char c){
	return (c == '{') || (c == '}') || (c == '[') || (c == ']') || (c == 't') 
	|| (c == 'f') || (c == 'n') || (c == '\"') || (c == ':') || (c == ',') ||
	start_of_number(c) || (c == ' ') || (c == '\n') || (c == '\t'); 
}
int valid_number(const CString *cs){
	if(cs->size == 1){
		return is_digit(cs->data[0]);
	}else if(cs->size == 2){
		if (cs->data[0] != '-')
			return is_non_zero_digit(cs->data[0]) && is_digit(cs->data[1]);	
		else
			return is_digit(cs->data[1]);
	}else{
		int cursor = 0;
		char neg = 0, exp = 0, exps = 0;
		char pnt = 0, leadz = 0, leadnz = 0;
		
		// -?      optional sign
		neg = cs->data[cursor] == '-';
		cursor += neg? 1 : 0;
		// 0 | [1-9]\d*    obligatory leading digit 
		leadz = cs->data[cursor] == '0';
		leadnz = is_non_zero_digit(cs->data[cursor]);
		
		if(leadnz){
			while(cursor < cs->size){
				if( ! is_digit(cs->data[cursor]) )
				 	break;
				cursor += 1 ;
			}
		}else if(leadz){
			if( (cursor < cs->size) && is_digit(cs->data[cursor+1]) ){
				return 0;
			}
			cursor += 1 ;
		}else
			return 0; // unrecognized leading char
		// (\.\d+)?     optional decimal point 
		if(cursor < cs->size){
 			pnt = (cs->data[cursor] == '.');
			if(pnt){
				cursor += 1;
				if((cursor < cs->size) && is_digit(cs->data[cursor])){
					while(cursor < cs->size){
						if( ! is_digit(cs->data[cursor]) )
							break;
						cursor += 1 ;
					}
				}else
					return 0; // no digit found after decimal point
			}
		}
		// ([eE][+-]?\d+)?     optional exponent part
		if(cursor < cs->size){
			exp = (cs->data[cursor] == 'E') || (cs->data[cursor] == 'e');
			if(!exp) return 0;
			cursor += exp? 1: 0;
			if(cursor < cs->size){
				exps = (cs->data[cursor] == '-') || (cs->data[cursor] == '+'); // optional +-
				cursor += exps? 1: 0;
				if((cursor < cs->size) && is_digit(cs->data[cursor])){ // one digit or more
					while((cursor < cs->size) && is_digit(cs->data[cursor++]));
				}else return 0;
			}else return 0; // missing after Ee number
		}
		if(cursor < cs->size)
			return 0; // unrecognized chars
	}
	return 1;
}
CString* next_number(const CString *src_ch, ulong index){
	uint len = 0;
	while( (len+index) < src_ch->size){
		if(!part_of_number(src_ch->data[len+index]))
			break;
		len++;
	}
	return sub_cstring(src_ch, index, index+len);
}
Token *new_token(TokenType ty, void *val, ulong line){
	Token *t = (Token*)malloc(sizeof(Token));
	memset(t, 0, sizeof(Token));
	t->type = ty;
	t->value = val;
	t->line_num = line;
	return t;
}
void free_token(Token *t){
	if(t != NULL){
		if(t->value != NULL){
			if(t->type == TKN_STRING)
				free_cstring((CString*)(t->value));
			else
				free(t->value);
		}
		free(t);
		t = NULL;
	}
}
void free_tokens_list(TokensList *list){
	Token *t = list->head;
	while(t != NULL){
		Token *q = t;
		t = t->next;
		free_token(q);
	}
	free(list);
}
TokensList *new_tokens_list(){
	TokensList *list = (TokensList*)malloc(sizeof(TokensList));
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
	return list;
}
void concat_token_lists(TokensList *l1, TokensList *l2){
	if( (l1 != NULL) && (l2 != NULL)){ // l1 & l2 exist
		if(l2->head != NULL){ // l2 not empty
			if(l1->head == NULL){ // l1 empty
				l1->head = l2->head;
			}else{ // l1 not empty
				l1->tail->next = l2->head;
				l1->size += l2->size;
			}
			l1->tail = l2->tail;
		}
		free(l2);
	}
}
void add_token_to_list(TokensList *list, Token *t){
	if(list->head == NULL){
		list->head = t;
		list->tail = t;
	}else{
		list->tail->next = t;
		list->tail = list->tail->next;
	}
	list->size++;
}
uint matches(char* keyword, const CString* cs, uint start){
	Garbage* gr = new_garbage();
	CString *keywordcs = w(gr, new_cstring_str(keyword), GrCString);
	CString *other_w = w(gr, sub_cstring(cs, start, start+keywordcs->size), GrCString);
	char eq = equals(keywordcs, other_w);
	ulong ksize = keywordcs->size;
	collect_and_dispose(gr);
	return eq?  ksize: 0;
}
CString* next_unknown(const CString* cs, uint start){
	int count = 1;
	while(! reserved_char(cs->data[start+count]) )
		count++;
	return sub_cstring(cs, start, start+count);
}
TokensList *tokenize(const CString *ch){
	TokensList *list = new_tokens_list();
	Garbage *gr = new_garbage();
	ulong i=0;
	ulong line = 1;
	while(i< ch->size){
		switch(ch->data[i]){
			case '{':
			{
				add_token_to_list(list, new_token(TKN_LBRACE, NULL, line));
				break;
			}
			case '}':
			{
				add_token_to_list(list, new_token(TKN_RBRACE, NULL, line));
				break;
			}
			case '[':
			{
				add_token_to_list(list, new_token(TKN_LBRACKET, NULL, line));
				break;
			}
			case ']':
			{
				add_token_to_list(list, new_token(TKN_RBRACKET, NULL, line));
				break;
			}
			case ':':
			{
				add_token_to_list(list, new_token(TKN_COLON, NULL, line));
				break;
			}
			case ',':
			{
				add_token_to_list(list, new_token(TKN_COMMA, NULL, line));
				break;
			}
			case '\"': // string
			{
				int start = i+1;
				int end = start;
				while((end < ch->size) && (ch->data[end] != '\"'))
					end += (ch->data[end] == '\\' )? 2 : 1;

				add_token_to_list(list, new_token(TKN_STRING, 
					unescape_special_chars(w(gr, sub_cstring(ch, start, end), GrCString))
				, line));
				i = end;
				
				break;
			}
			case ' ':
				break;
			case '\n':
				line += 1;
				break;
			case '\t':
				break;
			default:
			{
				if(start_of_number(ch->data[i])){// number
					CString *num_cs = next_number(ch, i);
					if(valid_number(num_cs)){
						double *v = malloc(sizeof(double));
						*v = todouble(num_cs);
						add_token_to_list(list, new_token(TKN_NUMBER, (void*)v, line));
						i += num_cs->size - 1;
					}else{
						CString* uk = next_unknown(ch, i);
						add_token_to_list(list, new_token(TKN_UNKNOWN, uk, line));
						i += uk->size - 1;
					}
					free_cstring(num_cs);
				}else if(ch->data[i] == 't'){ // true	
					uint l = matches("true", ch, i);
					if(l > 0){
						i += l - 1; // 1 is added down
						add_token_to_list(list, new_token(TKN_TRUE, NULL, line));
					}else{
						CString* uk = next_unknown(ch, i);
						add_token_to_list(list, new_token(TKN_UNKNOWN, uk, line));
						i += uk->size - 1;
					}
				}else if(ch->data[i] == 'f'){ // false
					uint l = matches("false", ch, i);
					if(l > 0){
						i += l - 1; // 1 is added down
						add_token_to_list(list, new_token(TKN_FALSE, NULL, line));
					}else{
						CString* uk = next_unknown(ch, i);
						add_token_to_list(list, new_token(TKN_UNKNOWN, uk, line));
						i += uk->size - 1;
					}
				}else if(ch->data[i] == 'n'){ // null
					uint l = matches("null", ch, i);
					if(l > 0){
						i += l - 1; // 1 is added down
						add_token_to_list(list, new_token(TKN_NULL, NULL, line));
					}else{
						CString* uk = next_unknown(ch, i);
						add_token_to_list(list, new_token(TKN_UNKNOWN, uk, line));
						i += uk->size - 1;
					}
				}else{
					CString* uk = next_unknown(ch, i);
					add_token_to_list(list, new_token(TKN_UNKNOWN, uk, line));
					i += uk->size - 1;
				}
			}
		}
		i++;
	}
	collect_and_dispose(gr);
	return list;
}
CString *untokenize(const TokensList *list){
	Garbage *gr = new_garbage();
	// calc size of chunk
	ulong size = 0;
	Token *t = list->head;
	while(t != NULL){
		if((t->type == TKN_LBRACE) || (t->type == TKN_RBRACE) || (t->type == TKN_COLON) || (t->type == TKN_COMMA)
			|| (t->type == TKN_LBRACKET) || (t->type == TKN_RBRACKET)){
			size += 1;
		}else if((t->type == TKN_NULL) || (t->type == TKN_TRUE)){
			size += 4;
		}else if(t->type == TKN_FALSE){
			size += 5;
		}else if(t->type == TKN_STRING){
			size += ((CString*)t->value)->size + 2; // 2 for quotes
		}else if(t->type == TKN_NUMBER){
			CString *ch = w(gr, new_cstring_flo(*((double*)t->value)), GrCString);
			size += ch->size;
		}
		t = t->next;
	}
	// allocate chunk
	CString* ch = new_cstring(size);
	ulong i = 0;
	// fill-in data
	t = list->head;
	while((t != NULL) && (i < ch->size)){
		switch(t->type){
			case TKN_LBRACE:
				ch->data[i++] = '{';
				break;  
			case TKN_RBRACE:
				ch->data[i++] = '}';
				break;
			case TKN_LBRACKET:
				ch->data[i++] = '[';
				break;
			case TKN_RBRACKET:
				ch->data[i++] = ']';
				break;
			case TKN_COMMA:
				ch->data[i++] = ',';
				break;
			case TKN_COLON:
				ch->data[i++] = ':';
				break;
			case TKN_NULL:
			{
				CString *nul_ch = w(gr, new_cstring_str("null"), GrCString);
				subcopy(ch, nul_ch, i, 0, nul_ch->size);
				i += nul_ch->size;
				break;
			}
			case TKN_TRUE:
			{
				CString *bool_ch = new_cstring_str("true");
				subcopy(ch, bool_ch, i, 0, bool_ch->size);
				i += bool_ch->size;
				break;
			}
			case TKN_FALSE:
			{
				CString *bool_ch = new_cstring_str("false");
				subcopy(ch, bool_ch, i, 0, bool_ch->size);
				i += bool_ch->size;
				break;
			}
			case TKN_STRING:
			{
				CString *val = ((CString*)t->value);
				ch->data[i++] = '"';
				subcopy(ch, val, i, 0, val->size);
				i += val->size;
				ch->data[i++] = '"';
				break;
			}
			case TKN_NUMBER:
			{
				CString *num_ch = w(gr, new_cstring_flo(*((double*)t->value)), GrCString);
				subcopy(ch, num_ch, i, 0, num_ch->size);
				i += num_ch->size;
				break;
			}
		}
		t = t->next;
	}
	collect_and_dispose(gr);
	return ch;
}

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

ParserReport *new_report(){
	ParserReport *pr = (ParserReport*)malloc(sizeof(ParserReport));
	pr->success = 1;
	pr->message = NULL;
	return pr;
}
Block* new_block(char is_lst){
	Block *b = (Block*)malloc(sizeof(Block));
	b->is_list = is_lst;
	b->first_child = NULL;
	b->last_child = NULL;
	b->num_children = 0;
	return b;
}
Node*  new_node(CString* key, NodeContentType vtype, void *value){
	Node * new_n = (Node*)malloc(sizeof(Node));
	new_n->member_of_list = 0;
	new_n->key = key;
	new_n->content_type = vtype;
	new_n->value = value;
	new_n->is_root = 0;
	new_n->next_sibling = NULL;
	return new_n;
}
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

BlockType top_block_type(const Stack *s){
	Block *b = (Block*)top(s);
	return (b != NULL) ? (b->is_list? BLOCK_LIST : BLOCK_OBJECT) : BLOCK_NONE ;
}
Node *fail(Garbage* garbage, ParserReport *report, 
			ErrNo errno, const Token *current,const Token *next){
	if(garbage != NULL)
		collect_and_dispose(garbage);
	if(report != NULL){
		Garbage* gr = new_garbage();
		report->success = 0;
		CString* msg = NULL;
		switch (errno){
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
							*dd = *((double*)q->value);
							chi = new_node(csclone(last_key), NCT_NUMBER, (void*)dd);
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
							*dd = *((double*)t->value);
							chi = new_node(NULL, NCT_NUMBER, (void*)dd);
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
			*dd = *((double*)node->value) ;
			add_token_to_list(list, new_token(TKN_NUMBER, dd, 0));
			break;
		}
	}
	return list;
}

Stack *new_stack(){
	Stack *s = (Stack*)malloc(sizeof(Stack));
	s->head = NULL;
	s->top = NULL;
	return s;
}
void push(Stack *s, void *p){
	StackNode *n = (StackNode*)malloc(sizeof(StackNode));
	n->payload = p;

	if (s->head == NULL) { // empty stack
		s->top = n;
		s->head = n;
	}else{
		s->top->next = n;
		s->top = s->top->next;
	}
}
void *top(const Stack *s){
	return (s->top == NULL)? NULL : s->top->payload;
}
void *pop(Stack *stack){
	if(stack->head == NULL)
		return NULL;
	StackNode *sn = stack->head;
	if(sn == stack->top){ // contains a single stacknode
		void *tmp = sn->payload;
		free(sn);
		stack->top = NULL;
		stack->head = NULL;
		return tmp;
	}
	// find stack node under top
	while(sn->next != stack->top)
		sn = sn->next;
	void *tmp = stack->top->payload;
	free(stack->top);
	sn->next = NULL;
	stack->top = sn;
	return tmp;
}
char is_stack_empty(const Stack *s){
	return ((s == NULL) || (s->head == NULL));
}
void free_stack(Stack *s){
	while ( !is_stack_empty(s) )
		pop(s);
	free(s);
}
