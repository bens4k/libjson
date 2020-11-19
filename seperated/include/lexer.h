#ifndef LEXER_H
#define LEXER_H

#include "cstring.h"

/*start of typedef*/
typedef enum { TKN_STRING, TKN_NUMBER, TKN_TRUE, TKN_FALSE, TKN_NULL, TKN_LBRACE, TKN_RBRACE, TKN_COLON, TKN_COMMA,
				TKN_LBRACKET, TKN_RBRACKET, TKN_UNKNOWN} TokenType;
typedef struct token Token;
typedef struct tokens_list TokensList;
/*end of typedef*/

/*start of struct defs*/
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
/*end of struct defs*/

/*start of func protos*/
TokensList *tokenize(const CString *cs);
CString *untokenize(const TokensList *list);
TokensList *new_tokens_list();
Token *new_token(TokenType ty, void *val, ulong line);
void add_token_to_list(TokensList *list, Token *t);
void free_tokens_list(TokensList *list);
void concat_token_lists(TokensList *l1, TokensList *l2);
CString* token_cstring(const Token *t);
CString* tokens_list_cstring(const TokensList *list);
/*end of func protos*/

#endif
