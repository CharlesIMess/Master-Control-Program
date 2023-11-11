/*
 * string_parser.h
 *
 *  Created on: Oct 15, 2024
 *      Author: Charles Ian Mess
 *
 *
 */

#ifndef STRING_PARSER_H_
#define STRING_PARSER_H_


typedef struct
{
    char** command_list;
    int num_token;
}command_line;

// count how many tokens in buf based on delim
int count_token (char* buf, const char* delim);

//This functions can tokenize a string to token arrays base on a specified delimeter,
//it returns a struct variable
command_line str_filler (char* buf, const char* delim);

//this function frees all the tokens within the array.
void free_command_line(command_line* command);


#endif /* STRING_PARSER_H_ */
