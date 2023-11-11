CFLAGS = -g -w -Wall

all: iobound cpubound string_parser.o mcp

mcp: mcp.o string_parser.o
	gcc $(CFLAGS) -o mcp mcp.o string_parser.o

mcp.o: mcp.c
	gcc $(CFLAGS) -c mcp.c

string_parser.o: string_parser.c
	gcc $(CFLAGS) -c string_parser.c

iobound : iobound.c
	gcc iobound.c -o iobound

cpubound : cpubound.c
	gcc cpubound.c -o cpubound

clean:
	rm -f *.o iobound cpubound mcp


