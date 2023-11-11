#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include "string_parser.h"
#define LINE_LEN 128
#define initial_size 20

// global variable to store the pid of the current running process
pid_t curr_pid;

void alarm_handler(int signum);

void display_proc(pid_t pid);

int main(int argc, char *argv[]){
    
    // check if the program is executed with the right number of args	
    if(argc != 2){
	printf("Error Usage: ./MCP input_file.txt\n");
	return 1;
    }
    
    // Declare a command_line to tokenize each command
    // Struct defined in string_parser.h 
    command_line tokens;
   
    // open and set the file ptr 
    FILE *filePtr;
    filePtr = fopen(argv[1], "r");
    // error check
    if(filePtr == NULL){
	perror("Error opening file:");
	return 1;
    }

    // initialize variables to use getline
    size_t len = LINE_LEN;
    char *line_buffer = malloc(LINE_LEN);
    size_t read;
    
    //declare and allocate a array of pid_t
    int size = initial_size;
    pid_t *pid = (pid_t*)malloc(initial_size * sizeof(pid_t));
    
    // declare variables to make signal mask 
    sigset_t set;
    int sig;

    // add SIGUSR1 to the signal set and block SIGUSR1 to create a signal mask
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &set, NULL);
    
    // reads each line from the input file and trys to execute the program in
    // a child process
    int line = 0;    
    while((read = getline(&line_buffer, &len, filePtr)) != -1){
	// skip if line is blank
	if(read == 1){
	    continue;
	}
	// tokenize the line_buffer using the str_filler function 
	// from the string_parser.c class
        tokens = str_filler(line_buffer, " ");

	// create a new child process and add it to the pid_t array
	pid[line] = fork();
	// error handling
	if(pid[line] < 0){
	    perror("Fork failed");
	    return 1;
	}
	// if it is the child, execute the program from this line in the input
	// file
	if(pid[line] == 0){
	    // wait for the SIGUSR1 signal to execute the process
	    sigwait(&set, &sig);
	    if(execvp(tokens.command_list[0], tokens.command_list) == -1)
	    {
		//error handling
		perror("Failed to execute process");
                exit(-1);
	    }
	}
	//free the array malloced in str_filler
        free_command_line(&tokens);
        
	// increase line(process) counter
	line++;
	// reallocate space in the pid_t if there are more lines then 
	// expected in the input file
        if(line == size){
		size*=2;
		pid = (pid_t*)realloc(pid,size*sizeof(pid_t));
		if(pid==NULL){
			perror("Failed to reallocate memory");
			return 1;
		}

	}
    }
    // set the signal function to listen for SIGALRM and to call
    // alarm handler when that happens
    signal(SIGALRM, alarm_handler);
    
    // Send each signal the SIGUSR1 to start each process.
    // Only let each process run for a time quantum of 2 seconds, then 
    // context switch. 
    for(int i = 0; i < line; i++){
	curr_pid = pid[i];
	kill(pid[i], SIGUSR1);
	printf("Start Process: %d\n", pid[i]);
	alarm(2);
	pause();
    }
    
    // Resume each process for a time quantum of 2 seconds and context switch.
    // Repeat untill each process has finished executing.
    // Alarm_handler will call display_proc to display information about
    // each process as it executes.
    int status;
    int terminated;
    while(1){
    terminated = 0;
    for(int i = 0; i < line; i++){
	curr_pid = pid[i];
	if(waitpid(pid[i], &status, WNOHANG) != 0){
	    if(WIFEXITED(status)){
	    terminated++;
	    continue;
	    }
	}
	kill(pid[i], SIGCONT);
	printf("Resume process: %d\n", pid[i]);
	alarm(2);
	pause();	
    }
    if(terminated == line){
	break;
    }
    }

    // free all dynamic memory
    free(line_buffer);
    fclose(filePtr);
    free(pid);
}

void alarm_handler(int signum){
    // stop the currently running process and display information about it
    kill(curr_pid, SIGSTOP);
    printf("Stop process: %d\n", curr_pid);
    display_proc(curr_pid);
}

void display_proc(pid_t pid){
   // read from the /proc file of each process and printf out important 
   // information.
   
   // declare and init the correct file name for the current process
   char filename[256];  
   sprintf(filename, "/proc/%d/status", pid);

   // delare and allocate space for a line buffer
   size_t len = 256;
   char *line_buffer = (char*)malloc(len);
   
   // open the /proc/pid/status file
   FILE* fp = fopen(filename, "r");
   // error handling
   if(fp == NULL){
	perror("Error opening /proc file:");
	exit(EXIT_FAILURE);
   }
   
   //parse each line of the /proc file and print relavent information.
   printf("Process information from last context switch:\n");
   command_line proc_status;
   while(getline(&line_buffer, &len, fp) != -1){
	char *copy = strdup(line_buffer);
	proc_status = str_filler(line_buffer, ":"); 
	char *pid_stat = strdup(proc_status.command_list[0]);
	//printf("%s\n", proc_status.command_list[1]);
	if(strcmp("Name", pid_stat) == 0){
	    printf("%s",copy);
	}	
	if(strcmp("State", pid_stat) == 0){
	    printf("%s",copy);
	}	
	if(strcmp("Pid", pid_stat) == 0){
	    printf("%s",copy);
	}	
	if(strcmp("PPid", pid_stat) == 0){
	    printf("%s",copy);
	}	
	if(strcmp("voluntary_ctxt_switches", pid_stat) == 0){
	    printf("%s",copy);
	}	
	if(strcmp("nonvoluntary_ctxt_switches", pid_stat) == 0){
	    printf("%s",copy);
	}
	if(strcmp("VmSize", pid_stat) == 0){
	    printf("%s",copy);
	}
        // free dynamically allocated memory.
	free(copy);
	free_command_line(&proc_status);
	free(pid_stat);
    }  
   free(line_buffer);
   fclose(fp); 
}

