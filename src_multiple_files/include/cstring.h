#ifndef CSTRING_H
#define CSTRING_H

/*start of typedef*/
typedef struct cstring CString;
typedef struct splits SplitsList;
/*end of typedef*/


/*start of struct defs*/
typedef struct cstring{
	char* data;
	unsigned long size;
} CString;

typedef struct splits{
	unsigned int size;
	CString** splits;
} SplitsList;
/*end of struct defs*/

/*start of func protos*/
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
CString  *strip_sequence(const CString* cs, const CString* seq);
CString  *strip_char(const CString* cs, char c);

SplitsList *new_splits_list(unsigned int c);
SplitsList *split(const CString *cs, char c);
CString    *join_splits(const SplitsList *list, char join_c);
void        free_splits_list(SplitsList *list);
/*end of func protos*/

#endif
