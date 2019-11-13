#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <string.h>


int main (void)
{
	pid_t child_pid;
	int status;
	
	char* env_vars[] = {
		"VAR1=variabile1",
		"HOME=boh",
		"questa_non_e_con_l_uguale",
		NULL
	};
	
	char* args[] = {
		"name_command",   /* */
		"VAR1",
		NULL
	};
	
	printf("%s: My PID is %d and my parent's PID is %d\n",
	       __FILE__, getpid(), getppid());
  
	/* Creating child processes that run an external command */
#define COMMAND "gcc test-env.c -o test-env"
/*#define COMMAND "gcc test-command-line.c -o test-command-line" */
/*#define COMMAND "gcc test-getpid.c -o test-getpid" */
	status = system(COMMAND);
	printf("COMMAND \"%s\" terminated with exit status %d\n",
	       COMMAND, status);
	
	
	execve("./char-loop",NULL,NULL);
	/*execve("./test-env", args, env_vars); */
	/*execve("./test-command-line", args, env_vars); */
 	/*execve("./test-getpid",NULL,NULL); */
	
	/* don't need to check success. If here not success */
	fprintf(stderr, "%s: %d. Error #%03d: %s\n",
		__FILE__, __LINE__, errno, strerror(errno));
	
	exit(EXIT_FAILURE);
}
