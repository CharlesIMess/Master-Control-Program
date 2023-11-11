#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

#define _GUN_SOURCE
#define MAX_COMMAND_SIZE 50

int count_token (char* buf, const char* delim)
{
	int count = 0;
	char *save_ptr;
	char *token;
	char *copy = strdup(buf);

	if(buf == NULL){
	    return 0;
	}

	token = strtok_r(copy, delim, &save_ptr);
	while(token != NULL){
	    token = strtok_r(NULL, delim, &save_ptr);
	    count++;
	}	
	free(copy);
	return count;	
}

command_line str_filler (char* buf, const char* delim)
{
	char * token;
	command_line line;
	char *save_ptr;

	buf = strtok_r(buf, "\n", &save_ptr);
	line.num_token = count_token(buf, delim);
		
	line.command_list = (char**)malloc(sizeof(char*)*(line.num_token+1));


	token = strtok_r(buf, delim, &save_ptr);
	int i = 0;
	while(token != NULL){
	    line.command_list[i] = strdup(token);
	    token = strtok_r(NULL, delim, &save_ptr);
	    i++;
	}

	line.command_list[line.num_token] = NULL;


	return line;
}

	


void free_command_line(command_line* command)
{
	for(int i = 0; i< command->num_token; i++){
	    free(command->command_list[i]);
	}

	free(command->command_list);
}
