#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	printf("Producer started at: %ld\n", tv.tv_usec);
	
	
	if (argc != 2) {
		printf("invalid argument count.\n");
		return -1;
	}
	
	int M = atoi(argv[1]);
	
	char alphanumerics[36] = {'a', 'b', 'c', 'd', 'e', 'f',
   				  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u',
   				  'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	
	
	srand(time(NULL));
	int i;
	
	for (i = 0; i < M; i++) {
		
		char random = alphanumerics[rand() % 36];
		printf("%c", random);
	}
	return 0;
}
