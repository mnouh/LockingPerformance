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
#include <sys/time.h>
#include <semaphore.h>
#include <sched.h>

#define DEBUG 0
#define MAX_SIZE 80

#ifndef min
#define min(a, b) (((a) < (b)) ? (a):(b))
#endif

typedef struct{
	unsigned long long int num1;
	unsigned long long int num2;
	pid_t winner;
	sem_t pMutex;
	int ready;
}Data;

int main(int argc, char* argv[]){
	
	if (DEBUG) printf("read in command line values\n");
	int numChildProc = atoi(argv[1]);
	unsigned long long int maxVal = atoi(argv[2]);
	
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
	
	if (sem_init(&myData->pMutex, 1, 1) < 0){
		printf("sem_init failed on parent: %s\n", strerror(errno));
		exit(2);
	}
	
	pid_t pid = getpid();
	int status;
	myData->num1 = 1;
	myData->num2 = 2;
	
	int i;
	myData->ready = 0;
	
	for (i = 0; i < numChildProc; i++){
		if (pid > 0) {
			if((pid = fork()) < 0){
				printf("Fork failed to execute: %s", strerror(errno));
				exit(2);
			}
		}
	}
	
	if (pid > 0){
		
		struct timeval pStart,  pEnd,  pLapsed; 
	
		gettimeofday (&pStart, NULL); 

		for(i = 0; i < numChildProc; i++){ 
			wait(&status);
		}
		
		gettimeofday (&pEnd, NULL); 
	
		pLapsed.tv_usec = pEnd.tv_usec - pStart.tv_usec; 
	
		printf("\nPart B with %d processes using sem_trywait() process runs in the following amount of time: %lu\n\n", numChildProc, pLapsed.tv_usec);
		
		if (DEBUG) printf("print out values\n");
		
		if (myData->num1 > myData->num2)
			printf("The greatest value is: %llu and the winner is: %d \n", myData->num1, myData->winner);
		else
			printf("The greatest value is: %llu and the winner is: %d \n", myData->num2, myData->winner);
	}
	 
		
	if (pid == 0){
		
		if (sem_wait(&myData->pMutex) == -1){
			printf("sem_wait() failed: %s", strerror(errno));
			exit(2);
		}

		myData->ready++;

		if (sem_post(&myData->pMutex) == -1){
			printf("sem_post() failed: %s", strerror(errno));
			exit(2);
		}

		while (myData->ready < numChildProc);
		
		pid_t kidpid;
		kidpid = getpid();
		struct timeval start,  end,  lapsed;
		unsigned long long int val3 = 0;
		

		printf("\n\t pid: %d coming out of while loop\n", kidpid);
		while (myData->num1 < maxVal && myData->num2 < maxVal) {
			
			gettimeofday(&start, NULL);
			
			if (sem_wait(&myData->pMutex) == -1){
				printf("sem_wait() failed: %s", strerror(errno));
				exit(2);
			}
			
			gettimeofday(&end, NULL);
			
			lapsed.tv_usec = end.tv_usec - start.tv_usec; 
	
			printf("\n\t\t[[Part B sem_try_wait() processes pid: %d busywait waits the following amount of time to aquire the lock: %lu]]\n", kidpid, lapsed.tv_usec);
	
			if (DEBUG) printf("\tcomputed value: %llu, maxVal: %llu\n", val3, maxVal);
		
			if(val3 < maxVal){
				val3 = myData->num1 + myData->num2;
				if (myData->num1 < myData->num2){
					myData->num1 = val3;
				}else{
					myData->num2 = val3;
				}
			
				myData->winner = getpid();
			}
			

			if (sem_post(&myData->pMutex) == -1){
				printf("sem_post() failed: %s", strerror(errno));
				exit(2);
			}
			
			if(sched_yield() == -1){
				printf("Error on sched_yield: %s", strerror(errno));
				exit(-1);
			}
		}
	}
	
	if (DEBUG) printf("Delete memory\n");	
	if (pid > 0){
		if((key = shmctl(shmID, IPC_RMID, &buffer)) == -1){
		   printf("shmctl failed: %s", strerror(errno));
		   exit(2);
		}
		sem_destroy(&myData->pMutex);
	}
		
	if((key = shmdt(myData)) == -1){
		printf("shmdt failed: %s", strerror(errno));
		exit(2);
	}
	
	return 0;
} 	
