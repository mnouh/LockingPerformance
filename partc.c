#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <unistd.h> 

#define DEBUG 1
#define MAXVALUE "1000000000000000000"


int runWaitTest(int numProcesses, int test){
	pid_t pid;
	
	if(test == 0){
		
		pid = fork();
		
		if(pid < 0){
			printf("(%d)Fork failed to execute\n", getpid());
			exit(-1);
		}else if (pid > 0){
			wait(NULL);
		}else if(pid == 0){
			char numProcCharArray[5];
			sprintf(numProcCharArray, "%d", numProcesses);
					
			if(execlp("./partb_busywait", "./partb_busywait", numProcCharArray, MAXVALUE, (char*) NULL) == -1){
				printf("\n\texeclp failed\n");
				exit(-1);
			}
		}
	}else if (test == 1){
		pid = fork();
	
		if(pid < 0){
			printf("(%d)Fork failed to execute\n", getpid());
			exit(1);
		}else if (pid > 0){
			wait(NULL);
		}else if(pid == 0){
			char numProcCharArray[5];
			sprintf(numProcCharArray, "%d", numProcesses);
					
			if(execlp("./partb_sched_yield", "./partb_sched_yield", numProcCharArray, MAXVALUE, (char*) NULL) == -1){
				printf("\n\texeclp failed\n");
				exit (-1);
			}
		}
	}else{
		pid = fork();
		
		if(pid < 0){
			printf("(%d)Fork failed to execute\n", getpid());
			exit(1);
		}else if (pid > 0){
			wait(NULL);
		}else if(pid == 0){
			char numProcCharArray[5];
			sprintf(numProcCharArray, "%d", numProcesses);
					
			if(execlp("./partb_sem", "./partb_sem", numProcCharArray, MAXVALUE, (char*) NULL) == -1){
				printf("\n\texeclp failed\n");
				exit (-1);
			}
		}
	}
	return 0;
}

int main(int argc, char* argv[]){
	int i = 0;
	
	for (i = 2; i <= 4; i += 2){
		runWaitTest(i, 0);
	}

	for (i = 2; i <= 4; i += 2){
		runWaitTest(i, 1);
	}

	for (i = 2; i <= 4; i += 2){
		runWaitTest(i, 2);
	}

	return 0;
} 	
