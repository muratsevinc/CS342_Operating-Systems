#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("invalid amount of inputs\n");
		return -1;
	}
	
	int M = atoi(argv[1]);
	
	int i;
	
	for (i = 0; i < M+28; i++) { // note that 28 is the char count of producer's time printing 
		char ch = 0;
		scanf("%c", &ch);
		printf("%c", ch);
	}
	
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	printf("\nConsumer finished at: %ld\n", tv.tv_usec);
	
	
	return 0;
}
