#include "../include/cstring.h"
#include "../include/stdlibs.h"
#include "../include/chars.h"

/*start of func defs*/
// allocate a csting of a specified size 
CString *new_cstring(unsigned long size){
	CString *cs = (CString*)malloc(sizeof(CString));
	if (cs == NULL) return NULL;
	cs->size = size;
	cs->data = (size == 0)? NULL : (char*)malloc(cs->size);
	return cs;
}
// create a cstring from a given c style string (char*) 
CString *new_cstring_str(const char* s){
	size_t size = strlen(s);
	CString *cs = new_cstring(size);
	if (cs == NULL) return NULL;
	// for(int i=0; i<cs->size ; i++)
	// 	cs->data[i] = s[i];
	memcpy(cs->data, s, size);
	return cs;
}
// create a cstring from a given single char
CString *new_cstring_char(char c){
	CString* cs = new_cstring(1);
	if (cs == NULL) return NULL;
	cs->data[0] = c;
	return cs;
}
// create a cstring from a given integer
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
	if (int_ch == NULL) return NULL;
	for(int i = length-1;  i >= end_index ; i--){
		int k = ((a%cur_b - a%prev_b) / prev_b );
		int_ch->data[i] =  digit_to_char( (int)k );
		prev_b = cur_b;
		cur_b *= 10;
	}
	if(sign) int_ch->data[0] = '-';
	return int_ch;
}
// create a cstring from a given float
CString *new_cstring_flo(double fl){
	size_t size = snprintf(NULL, 0, "%lf", fl);
	size++;
	char* buff = malloc(size);
	if (buff == NULL) return NULL;
	sprintf_s(buff, size, "%lf", fl);
	CString* double_ch = new_cstring_str(buff);
	free(buff);
	return double_ch;
}
void free_cstring(CString *cs){
	if(cs->size > 0)
		free(cs->data);
	free(cs);
}
// remove the '\q' used to replace '\"' 
static void uncover_escaped_quotes(CString *cs){
	for(uint i=0; i< cs->size ; i++){
		if(cs->data[i] == '\\'){
			if(((i+1) < cs->size) && (cs->data[i+1] == 'q')){
				cs->data[i+1] = '"';
			}
		}
	}
}
// replace '\"' with '\q'
static void cover_escaped_quotes(CString *cs){
	for(uint i=0; i< cs->size ; i++){
		if(cs->data[i] == '\\'){
			if(((i+1) < cs->size) && (cs->data[i+1] == '"')){
				cs->data[i+1] = 'q';
			}
		}
	}
}
// copy a cstring into another with starting points for both src and dest 
ulong subcopy(CString *dest, const CString *src, unsigned long dest_start,
				unsigned long src_start, unsigned long amount){
	ulong amount_copied = 0;
	// ensure starts are in bounds of cstrings
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
// convert a cstring to double 
double todouble(const CString *cs){
	char* strd = str(cs);
	double v = strtod(strd, NULL);
	free(strd);
	return v;
}
// convert a cstring to c style string (char*)
char*  str(const CString *cs){
	char *s = NULL;
	if(cs->size > 0){
		size_t str_size = cs->size;
		str_size++; // null terminated string
		s = (char*)malloc(str_size);
		if (s == NULL) return NULL;
		for(size_t i=0; i< str_size-1 ; i++)
			s[i] = cs->data[i];
		s[str_size-1] = '\0';
	}
	return s;
}
// allocate a linked list that holds multiple cstrings
// used as a return for splitting a single cstring
SplitsList* new_splits_list(unsigned int size){
	SplitsList* list = (SplitsList*)malloc(sizeof(SplitsList));
	if (list == NULL) return NULL;
	list->size = size;
	list->splits = (CString**)malloc((list->size)*sizeof(CString*));
	return list;
}
// split a cstring into multiple cstrings at locations where the given char c is found
// ex: split(cstring("hello, world"), ',') -> ["hello"," world"]
SplitsList* split(const CString *cs, char c){
	uint count = 1; // count always 1 or greater
	for(ulong i=0; i< cs->size; i++){	// calc number of sub-cstring
		if(cs->data[i] == c)
			count++;
	}
	SplitsList *list = new_splits_list(count);
	if (list == NULL) return NULL;
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
	//adding last subcstring
	CString *subcs = sub_cstring(cs, splits_cursor, cs->size);
	list->splits[k++] = subcs;
	return list;
}
void free_splits_list(SplitsList *list){
	for(uint i=0; i< list->size; i++){
		free_cstring(list->splits[i]);
	}
	free(list->splits);
	free(list);
}
// create a copy of a cstring
CString *csclone(const CString *cs){
	CString *ncs = NULL;
	if(cs != NULL){
		ncs = new_cstring(cs->size);
		if (ncs == NULL) return NULL;
		for(uint i=0; i< cs->size; i++)
			ncs->data[i] = cs->data[i];
	}
	return ncs;
}
// returns sub-cstring, start-inclusive, end-exclusive
CString *sub_cstring(const CString *cs, long start, long end){
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
		if (ncs == NULL) return NULL;
		ulong j = 0;
		for(ulong i=e_start; i<e_end; i++){
			ncs->data[j++] = cs->data[i];
		}
	}
	return ncs;
}
// remove all occurances of given char from a cstring
CString *strip_char(const CString* cs, char c){
	CString *stripped_cs;
	uint count = 0;
	for(uint i = 0; i< cs->size ; i++){
		if(cs->data[i] == c)
			count++;
	}
	if( count > 0){
		stripped_cs = new_cstring(cs->size - count);
		if (stripped_cs == NULL) return NULL;
		uint j = 0;
		for(uint i = 0; i< cs->size ; i++){
			if(cs->data[i] != c)
				stripped_cs->data[j++] = cs->data[i];
		}
	}else 
		stripped_cs = csclone(cs);
	return stripped_cs;
}
//remove all occurences of seq in cs
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
// replace all special chars of form "\n" into the real value '\n'
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
		if (esc_ch == NULL) return NULL;
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
// return a lowercase copy of cs
CString *lowercase(const CString *cs){
	CString *lowercase_cs = csclone(cs);
	for(uint i=0; i< cs->size; i++)
		lowercase_cs->data[i] = lower_case(cs->data[i]);
	return lowercase_cs;
}
// returns a new cstring of value: c1 + cs + c2 
CString *enclose(const CString *cs, char c1, char c2){
	if(cs != NULL){
		CString *enclosed = new_cstring(cs->size + 2);
		if (enclosed == NULL) return NULL;
		enclosed->data[0] = c1;
		for(int i=0; i<cs->size; i++)
			enclosed->data[i+1] = cs->data[i];
		enclosed->data[enclosed->size - 1] = c2;
		return enclosed;
	}
	return NULL;
}
// returns a new cstring of value: cs + delimiter + s  // or cs + s if delimiter is 0
CString *joinstr(const CString *cs, char* s, char delimiter){
	CString* cs_str = new_cstring_str(s);
	if (cs_str == NULL) return NULL;
	CString* joinedcs = join(cs, cs_str, delimiter);
	free_cstring(cs_str);
	return joinedcs;
}
// returns a new cstring of value : cs1 + delimiter + cs2 // or cs1 + cs2 if delimiter is 0
CString *join(const CString *cs1, const CString *cs2, char delimiter){
	CString *joined = NULL;
	ulong size1 = (cs1 != NULL) ? cs1->size : 0;
	ulong size2 = (cs2 != NULL) ? cs2->size : 0;
	joined = new_cstring(size1 + size2 + 1*(delimiter != 0));
	if (joined == NULL) return NULL;
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
// returns a cstring containing all cstrings in list seperated by given delimiter
CString *join_splits(const SplitsList *list, char delimiter){
	// calculate total size, and allocate space
	ulong total_size = 0;
	for(uint i=0; i< list->size; i++)
		total_size += list->splits[i]->size;
	if(delimiter != 0)
		total_size += list->size - 1;
	CString *j_cs = new_cstring(total_size);
	if (j_cs == NULL) return NULL;
	ulong k = 0;
	for(uint i=0; i< list->size; i++){
		for(uint j=0; j< list->splits[i]->size; j++)
			j_cs->data[k++] = list->splits[i]->data[j];
		if((delimiter != 0) && (i < list->size - 1))
			j_cs->data[k++] = delimiter;
	}
	return j_cs;
}
/*end of func defs*/