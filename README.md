# Master-Control-Program

In this project I implemented a master control program (MCP for short) whose primary
job is to launch a pool of sub-processes that will execute commands given in an input file. The
MCP will read a list of commands (with arguments) to run from a file, it will then start up and
run a process that will execute the command. It will schedule the processes to run
concurrently in a time-sliced manner. In addition, the MCP will monitor the processes, keeping
track of how the processes are using system resources. 
