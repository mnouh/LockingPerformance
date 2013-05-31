#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>

#define DEBUG 0
#define MAX_SIZE 80

typedef struct{
	long long int num1;
	long long int num2;
	pid_t winner;
	int ready;
}Data;

int main(int argc, char* argv[]){
	
	if (DEBUG) printf("read in command line values\n");
	int numChildProc = atoi(argv[1]);
	long long int maxVal = atoi(argv[2]);
	
	if (DEBUG) printf("numChildProc: %d, maxVal: %llu\n", numChildProc, maxVal);
	
	if (DEBUG) printf("create myData struct\n");
	Data* myData;
	
	if (DEBUG) printf("initialized myData values\n");
	
	struct shmid_ds buffer;
		
	key_t key;
	int shmFlag;
	int shmID;
	size_t size;
	
	if (DEBUG) printf("Check argument count\n");
	
	if (argc < 3){
		printf("please use two integer values\n");
		exit(2);
	}
	
	if ((key = ftok("/proc", 'A')) == -1) {
		printf("Key creation failed: %s\n", strerror(errno));
		exit(1);
	}
	
	size = sizeof(*myData);
	shmFlag = 0644 | IPC_CREAT;
	//Error check
	if ((shmID = shmget(key, size, shmFlag)) == -1) {
		perror("shmget: shmget failed"); 
		exit(1); 
	}
	
	if ((myData = shmat(shmID, NULL, shmFlag)) == (void *) -1) {
		printf("shmat: shmat failed: %s\n", strerror(errno));
		exit(2);
	}
	
	pid_t pid = getpid();
	int status;
	myData->num1 = 1;
	myData->num2 = 2;
	
	int i;
	myData->ready = 0;
	if (DEBUG) printf("Create parent process\n");
	for (i = 0; i < numChildProc; i++){
		if (pid > 0) {
			if((pid = fork()) < 0){
				printf("Fork failed to execute: %s", strerror(errno));
				exit(2);
			}
		}
	}
	
	
	if (pid == 0){
		myData->ready++;
		while (myData->ready < numChildProc);
		
		long long int val3 = 0;
		
		if (DEBUG) printf("%d :enter child while loop\n", getpid());
		while (myData->num1 < maxVal && myData->num2 < maxVal) {
			
			if (DEBUG) printf("\t %d : computed value: %llu, maxVal: %llu\n", getpid(), val3, maxVal);
			
			val3 = myData->num1 + myData->num2;
			if (myData->num1 < myData->num2){
				myData->num1 = val3;
			}else{
				myData->num2 = val3;
			}
			
			myData->winner = getpid();
			
			if(sched_yield() == -1){
				printf("Error on sched_yield: %s", strerror(errno));
				exit(-1);
			}
	
		}
		
		if (DEBUG) printf("Exit child\n");
	}
	
	if (pid > 0){
		if (DEBUG) printf("Wait for child processes\n");
		
		for(i = 0; i < numChildProc; i++){
			wait(&status);
		}
		
		
		if (DEBUG) printf("print out values\n");
		
		if (myData->num1 > myData->num2)
			printf("The greatest value is: %llu and the winner is: %d \n", myData->num1, myData->winner);
		else
			printf("The greatest value is: %llu and the winner is: %d \n", myData->num2, myData->winner);
	}
	
	if (pid > 0){
		if (DEBUG) printf("Delete memory\n");
		if((key = shmctl(shmID, IPC_RMID, &buffer)) == -1){
		   printf("shmctl failed: %s", strerror(errno));
		   exit(2);
		}
	}
		
	if((key = shmdt(myData)) == -1){
		printf("shmdt failed: %s", strerror(errno));
		exit(2);
	}
	
	return 0;
} 	
