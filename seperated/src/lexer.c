#include "../include/lexer.h"
#include "../include/stdlibs.h"
#include "../include/chars.h"
#include "../include/garbage.h"

/*start of func defs*/
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
/*end of func defs*/