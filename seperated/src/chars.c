#include "../include/chars.h"

/*start of func defs*/
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
/*end of func defs*/