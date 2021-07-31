#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

	
/* Linked List Node Definition */
struct Node {
	int threadIndex;
	int burstIndex;
	int burstLength;
	int timeWhenGenerated;
	struct Node *next;

};

/* Global Variable: head */
struct Node *head = NULL; 

/* Function: Insert to the beginning of the list */
void insert(int t_index, int b_index, int b_length, int time) {
	struct Node *new = (struct Node*) malloc(sizeof(struct Node));
	
	new->threadIndex = t_index;
	new->burstIndex = b_index;
	new->burstLength = b_length;
	new->timeWhenGenerated = time;
	
	new->next = head;
	head = new; 
}

/* Function: Prints the whole list */
void printList() {
	struct Node* cur = head;
	
	while (cur) {
		printf("Thread Index: %d%s%d%s%d%s%d%s", cur->threadIndex, "\tBurst Index ", cur->burstIndex, 
		"\tBurst Length ", cur->burstLength, "\tTime Generated ", cur->timeWhenGenerated, "\n");
		cur = cur->next;
	}
}



/* Function: Deletes a particular index */
void delete(int index) {

	if (index < 0)
		return;
		
	struct Node* cur = head;
	struct Node* prev = head;
	
	if (index == 0) {
		head = head->next;
		free(cur);
		return;
	}	
	
		
	
	while (index && cur->next ) {
		prev = cur;
		cur = cur->next;
		index--;
		
		if (!index){
			prev->next = cur->next;
			free(cur);
		
		}
	}
}



/* Function: Retrieves the index of a particular item */
int retrieveIndex(int key) {

	struct Node* cur = head;
	
	int counter = 0;
	
	while(cur) {
	
		if (cur->threadIndex == key) {
			return counter;	
		}
		
		counter++;
		cur = cur->next;
	}
	return -1;
}


	/* End of Linked List Part */
	
	
	
	/* Global Variables */

	int threadAmount;
	int burstAmount;
	int minBurstTime;
	int avgBurstTime;
	int minWaitingTime;
	int avgWaitingTime; 
	char* alg;
	char* inprefix;
	double* VRunTime;
	
	pthread_mutex_t lock;
	pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

	
	int totalWaitingTime; // for experiments
	int argCount;
	/* Random Exponential Distribution Functions */
	
int randBurstDist() {
	int burstTime = 0;
	while (burstTime < minBurstTime) {

		double lambda = 1 / (double)avgBurstTime;
		double random = (double) rand() / ((double)RAND_MAX + 1);
		double burst = -log(1 - random) ;
		burst /= lambda;
		
		burstTime = ceil(burst);
	}
	return burstTime;
}

int randWaitingDist() {
	int burstTime = 0;
	while (burstTime < minWaitingTime) {

		double lambda = 1 / (double)avgWaitingTime;
		double random = (double) rand() / ((double)RAND_MAX + 1);
		double burst = -log(1 - random) ;
		burst /= lambda;
		
		burstTime = ceil(burst);
	}
	return burstTime;


}	


/* Function: This is the function that decides which
	      node to delete. It returns the burst time
	      of that node to provide sleep information
	      to the Schedular Thread. */
	   
int deleteFirstComer() {
	
	struct Node* cur = head;
	if (cur == NULL)
		return -1;
	else {
		
		
		struct timeval currentTime;
		gettimeofday(&currentTime, NULL);	
	
		int min; 
		int count = 0;
		int index = 0;
		int length = cur->burstLength;
		
		int waitingTime = 0;
		
		if (cur->next == NULL) {
			waitingTime = currentTime.tv_usec - cur->timeWhenGenerated;
			length = cur->burstLength;
			
			
			if (strcmp(alg, "VRUNTIME") == 0)
				VRunTime[cur->threadIndex - 1] += cur->burstLength * ( 0.7 + 0.3 * cur->threadIndex);
			
			
			delete(0);
			return length;
		} else {
		
			
			
			if (strcmp(alg, "FCFS") == 0) {
				min = cur->timeWhenGenerated; 		// this FCFS algorithm finds the minimum "Generated Time"
				while(cur->next) {				// so that it deletes the first comer.
					count++;
					cur = cur->next;
					if (cur->timeWhenGenerated < min) {
					
						waitingTime = currentTime.tv_usec - cur->timeWhenGenerated;
						
						min = cur->timeWhenGenerated;
						index = count;	
						length = cur->burstLength;
					}
				}
			} else if (strcmp(alg, "SJF") == 0) {			// this SJF algorithm finds the minimum "Burst Time"
				min = cur->burstLength;			// so that it deletes the shortest job first.
				while(cur->next) {
					count++;
					cur = cur->next;
					if (cur->burstLength < min) {
					
						waitingTime = currentTime.tv_usec - cur->timeWhenGenerated;
					
						min = cur->timeWhenGenerated;
						index = count;	
						length = cur->burstLength;
					}
				}
				
			
			} else if (strcmp(alg, "PRIO") == 0) {		// this PRIO algorithm finds the threads with the lowest index
										// so that it deletes the most important job first.
				min = cur->threadIndex;			
				while(cur->next) {				
					count++;
					cur = cur->next;
					if (cur->threadIndex < min) {
					
						waitingTime = currentTime.tv_usec - cur->timeWhenGenerated;
					
						min = cur->threadIndex;
						index = count;	
						length = cur->burstLength;
					}
				}
			
			} else { // VRUNTIME last option
				
				cur = head;
				struct Node* temp;
				
				int threadToExecute = cur->threadIndex;
				int nextThread = 0;
				
				
				
				/* Find the minimum VRunTime */ 
				while (cur) {
					if (cur->next) {
						temp = cur->next;
						nextThread = temp->threadIndex;
					} else {
						nextThread = cur->threadIndex;
					
					}
						
				
					threadToExecute = cur->threadIndex;
					
					
					if (VRunTime[threadToExecute] > VRunTime[nextThread]) {
					
						threadToExecute = nextThread;
					}
				
					cur = cur->next;
					
				}

				// test - printf("Minimum v runtime is thread %d\n", threadToExecute);
				
				cur = head;
				
				while(cur) {				

					if (cur->threadIndex == threadToExecute) {
						waitingTime = currentTime.tv_usec - cur->timeWhenGenerated;
					
						VRunTime[threadToExecute] += cur->burstLength * ( 0.7 + 0.3 * cur->threadIndex);
						// test - printf("VRunTime is set as %f of thread %d\n", VRunTime[threadToExecute], threadToExecute) ;
						index = count;	
						length = cur->burstLength;
						break;
					}
					count++;
					cur = cur->next;
				}
			}
			
		}
		
		
		totalWaitingTime += waitingTime;
		delete(index);
		return length;
	}
}
	
	/* Thread Functions */
	
/* Function: W-Thread */
void* wThread(void* threadIndex) {

	int tIndex = *(int *) threadIndex;
	// test - printf("This is thread %d\n", tIndex);
	
	
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	
	/* Burst creation and insertion */
	
	for (int i = 0; i < burstAmount; i++) {
		usleep(randWaitingDist()); 				// A thread will sleep first for a random amount of time
		
		int burstTime = randBurstDist(); 			// Burst length is produced
		
		int generatedTime = currentTime.tv_usec ; 	
		
		
		
		
		/* Insertion to the ready queue */
		
		pthread_mutex_lock(&lock); 				// Get the lock before the critical section
		
		insert(tIndex, i + 1, burstTime, generatedTime); 	// Insertion to the ready queue. | Note: i + 1 is burst index
		
		if (head->next == NULL)
			pthread_cond_signal(&condition);			// Send signal to S-Thread 
		
		pthread_mutex_unlock(&lock);				// Release the lock back
		
		// test - printf("Added node with: %d %d %d %d\n", tIndex, i + 1, burstTime, generatedTime);
	}
	
	
	
	pthread_exit(0);
}

/* Function: S-Thread */


void* sThread(void* parameter) {

	int done = 0;
	int listSize = threadAmount * burstAmount;
	int counter = 0;
	

	
	while (counter < listSize || argCount == 5) {  
		pthread_mutex_lock(&lock); 		// Get the lock back, deleteFirstComer() needs the lock
	
		while (head == NULL && !done) {
			
			pthread_cond_wait(&condition, &lock);
			
			done = (counter == listSize);
		}
		pthread_mutex_unlock(&lock);
		pthread_mutex_lock(&lock); 		// Get the lock back, deleteFirstComer() needs the lock
		counter++;
		
		int sleepAmount2 = deleteFirstComer(); // this function deletes the node and returns the burst length
		// test - printf("Deleted. Sleep Amount: %d\n", sleepAmount2);
		pthread_mutex_unlock(&lock);		// Unlock before sleeping to enable W-Threads to proceed
		usleep(sleepAmount2);
			
	}

	
	
	// test - printf("S thread left\n");
	pthread_exit(0);

}

void* wThreadFromFile(void* threadIndex) {

	int tIndex = *(int *) threadIndex;

	
	char fileName[128];
	char threadNo[2];
	
	strcpy(fileName, inprefix);
	strcat(fileName, "-");

	sprintf(threadNo, "%d", tIndex);
	strcat(fileName, threadNo);
	strcat(fileName, ".txt");
			
	printf("Reading %s\n", fileName);
			
	char fileContent[2048];

	int burstNo = 1;
			
	FILE *fileptr;
			
	if((fileptr = fopen(fileName, "r")) == NULL ) {
		printf("Error: cannot open file.\n");
		return NULL;
	}
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
			
	while (fgets(fileContent, 2048, fileptr) != NULL) {

				
		int spaceIndex = 0;
		int newLineIndex = 0;
		while (fileContent[spaceIndex] != ' ') {
			spaceIndex++;
		}
		while (fileContent[newLineIndex] != '\n') {
			newLineIndex++;
		}
				
		char arg1[spaceIndex];
		char arg2[newLineIndex - spaceIndex];
				
				
		strncpy(arg1, fileContent, spaceIndex );
		strncpy(arg2, fileContent + spaceIndex, newLineIndex - spaceIndex );
		

		usleep(atoi(arg2));

		burstAmount = atoi(arg1);
		
		int generatedTime = currentTime.tv_usec;
		
		
		pthread_mutex_lock(&lock);
		insert(tIndex, burstNo, burstAmount, generatedTime);
		
		if (head->next == NULL)
			pthread_cond_signal(&condition);			// Send signal to S-Thread 
			
		pthread_mutex_unlock(&lock);
		
		
		burstNo++;
			
	}
	
	pthread_exit(0);
}

int main(int argc, char *argv[]) {

	
	
	/* Handling wrong arguments */ 
	if (!(argc == 8 || argc == 5)) {
		printf("Dear TA\nInvalid amount of arguments\nYour argument count: %d\nExpected: 5 or 8 arguments\n", argc);
		printf("Please use either one of these formats:\n");
		printf("\t./schedule <N> <Bcount> <minB> <avgB> <minA> <avgA> <ALG> or \n");
		printf("\t./schedule <N> <ALG> -f <inprefix> \n");
		return -1;
	}
	
	printf("Welcome to \"Multi-Threaded Scheduling Simulator\"\n\n");




	if (argc == 8) {
		argCount = 8;
		threadAmount = atoi(argv[1]);
		burstAmount = atoi(argv[2]);
		minBurstTime = atoi(argv[3]);
		avgBurstTime = atoi(argv[4]);
		minWaitingTime = atoi(argv[5]);
		avgWaitingTime = atoi(argv[6]);
		alg = argv[7];

		
		/* Handling the corner cases */
		if (threadAmount < 1 || threadAmount > 10) {
			printf("Thread amount, N, must be in between 1 and 10\n" );
			return -1;
		}
		
		if (minBurstTime < 100 || minWaitingTime < 100) {
			printf("minA and minB must be >= 100\n" );
			return -1;
		}
		
		if (strcmp(alg, "FCFS") != 0 && strcmp(alg, "SJF") != 0 && strcmp(alg, "PRIO") != 0 && strcmp(alg, "VRUNTIME") != 0){
			printf("Invalid algorithm. It must be one of these: FCFS, SJF, PRIO, VRUNTIME\n");
			return -1;
		}
		
		if (avgBurstTime < minBurstTime || avgWaitingTime < minWaitingTime) {
			printf("Average values cannot be smaller than minimum values.\n");
			return -1;
		}
		/* Input is valid */
		
		
		
		srand(time(NULL)); // random generator initialized
		totalWaitingTime = 0;	// experiment part
		
		/* VRunTime Array Initialization */
		double temp[threadAmount];
		VRunTime = temp;
		for (int i = 0; i < threadAmount; i++) {
			VRunTime[i] = 0; 	// They all are initially zero.
		}
		
		/* Mutex Lock Initialization */
		
		if (pthread_mutex_init(&lock, NULL) != 0) {
			printf("Mutex initialization failed.\n");
			return -1;
		}
		
		
		/* Thread Creation */
		
		pthread_t tidS;
		pthread_create(&tidS, NULL, sThread, NULL);
		
		pthread_t tid[threadAmount];
		int indexArray[threadAmount];
		
		for (int i = 1; i <= threadAmount; i++) {
			
			indexArray[i-1] = i;
			
			if (pthread_create(&tid[i-1], NULL, wThread, &indexArray[i-1]) != 0)
				printf("Error in thread creation.\n");
		}
		
		/* Waiting for Threads */
		
		for (int i = 1; i <= threadAmount; i++) {
			if (pthread_join(tid[i-1], NULL))
				printf("Error in thread join.\n");
		}
		
		pthread_join(tidS, NULL);
		
		printf("Simulation is over. Total Waiting Time of %s: %d\n", alg, abs(totalWaitingTime));
		
		pthread_mutex_destroy(&lock);
	
	} else { // it has to be 5.
		argCount = 5;
		threadAmount = atoi(argv[1]);
		alg = argv[2];
		inprefix = argv[4];
		
		int indexArray[threadAmount];
		
		if (strcmp(argv[3], "-f") != 0) {
			printf("\n\nDear TA\nPlease use the format: ./schedule <threadAmount> <algorithm> -f <inprefix>)\n");
			return -1;
		}

		pthread_t tidS;
		pthread_create(&tidS, NULL, sThread, NULL);
		
		pthread_t tid[threadAmount];
		
		for (int i = 1; i <= threadAmount; i++) {
			
			indexArray[i-1] = i;
			
			if (pthread_create(&tid[i-1], NULL, wThreadFromFile, &indexArray[i-1]) != 0)
				printf("Error in thread creation.\n");
		}
		
		for (int i = 1; i <= threadAmount; i++) {
			if (pthread_join(tid[i-1], NULL))
				printf("Error in thread join.\n");
		}
		
		printf("Simulation is over.\n");
		// pthread_join(tidS, NULL);	
	}
	return 0;
}
