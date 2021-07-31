#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>


int main(int argc, char *argv[])
{
	char command[70] = "";
	int mode = 1; // default is normal mode.
	int amount;
	printf("Welcome to myShell! Enter your command: (write -1 to exit.)\n");
	
	if (argc == 3 && !strcmp(argv[2],"2")) {
		
		sscanf(argv[1], "%d", &amount);
	
		printf("Current Mode is set as 'Tapped Mode'. Chars will be read %d by %d\n", amount, amount );
		mode = 2;	
	}
	else
		printf("Current Mode is set as 'Normal Mode'\n");
	char arguments[10][25];
	while (1) {
		printf("$isp>");
		memset(arguments, 0, sizeof arguments);
		memset(command, 0, sizeof command);
		fgets(command, 70, stdin);

		if (!strcmp(command, "-1\n"))
			break;
	
		if (!strcmp(command, "\n"))
			continue;

		
		int i = 0;
		
		int argumentAmount = 0;
		int beginningIndexOfArg = 0;
		
		
		
		while (i < 70 ) {
		
			
			if (command[i] == ' ') {
				
				
				argumentAmount++;
				beginningIndexOfArg = 0;
			}
			else {
				if (command[i] != 0 && command[i] != '\n') {
					
					arguments[argumentAmount][beginningIndexOfArg] = command[i];

				
					beginningIndexOfArg++;
				}
			}
			i++;
		}

		// argument seperation is completed. arguments[argNo][argString]  
		
		int containsPipe = 0; // boolean

		int k;
		for (k = 1; k < argumentAmount; k++) {
			if (!strcmp( arguments[k], "|")) 
				containsPipe = 1;	
		}
		
		if (containsPipe) {// input with 2 command: 2 child needed
			char* command1[25]; 
			char* command2[25];
				
			int item1 = 0;
			while (strcmp( arguments[item1], "|")) {
				command1[item1] = arguments[item1];	
				item1++;
			}	
			int temp = item1;
			item1++; // passing argument "|"
				
			int item2 = 0;
				
			for (; item1 <= argumentAmount; item1++) {
				
				command2[item2] = arguments[item1];
				item2++;
			}
				

			command1[temp] = NULL;
			command2[item2] = NULL;
			
			// argument allocation completed on command1 & command2.
		
			if (mode != 2) {

				int fd[2];
				
				if (pipe(fd) == -1)
					return -1;
					
					
				int pid1 = fork();
				
				if (pid1 < 0) {
					return -1;
				}
				
				if (pid1 == 0) {
				
					dup2(fd[1], 1);
					close(fd[0]);
					close(fd[1]);

					//execv(path1, command1);
					if (execvp(command1[0], command1) < 0) {
						printf("\nError encountered.\n");
						exit(0);
					}
				}

				int pid2 = fork();
				
				if (pid2 <0)
					return -1;
					
					
				if (pid2 == 0) {
				
					dup2(fd[0], 0);
					close(fd[0]);
					close(fd[1]);
					
					//execv(path2, command2);
					if (execvp(command2[0], command2) < 0 ) {
						printf("\nError encountered.\n");
						exit(0);
					}
				}
			
				close(fd[0]);
				close(fd[1]);
			
				waitpid(pid1, NULL, 0);
				waitpid(pid2, NULL, 0);
				
				memset(command1, 0 , sizeof (command1));
				memset(command2, 0 , sizeof (command2));

			}else{
				//printf("This is tapped mode!\n");
				
				
				int transferredBytes = 0;
				int readCount = 0;
				int writeCount = 0; 
				
				int pipe1fd[2];
				int pipe2fd[2];
				
				if (pipe(pipe1fd) == -1)
					return -1;
					
				if (pipe(pipe2fd) == -1)
					return -1;
			
				int pid1 = fork();
				
				if ( pid1 < 0) {
					printf("Fork problem.\n");
					return -1;
				}
				if (pid1 == 0) { // child 1 
					close(pipe2fd[0]);
					close(pipe2fd[1]);
					close(pipe1fd[0]);
					
					dup2(pipe1fd[1], 1);
					
					if (execvp(command1[0], command1)) {
						printf("\nError encountered.\n");
						exit(0);
					}
				} else {
				
					int pid2 = fork();
					
					if (pid2 < 0) {
						printf("Fork problem.\n");
						return -1;					
					} else if (pid2 == 0) {
					
						close(pipe1fd[0]);
						close(pipe1fd[1]);
						close(pipe2fd[1]);
					
						dup2(pipe2fd[0], 0);
						
						if (execvp(command2[0], command2) < 0 ) {
						
						printf("\nError encountered.\n");
						exit(0);
						
						}
					} else {
						close(pipe1fd[1]);
						close(pipe2fd[0]);
						
						int processedBytes;
						char buffer[amount];
						
						while( ( processedBytes = read(pipe1fd[0], buffer, amount)) > 0) {
							
							int writtenBytes = write(pipe2fd[1], buffer, processedBytes);
							
							transferredBytes += processedBytes + writtenBytes;
							readCount++;
							writeCount++; 
						}
						readCount++; // while checks and exits.
						
						
						
						close(pipe1fd[0]);
						close(pipe2fd[1]);
						
						wait(NULL);
						wait(NULL);
						
						memset(command1, 0 , sizeof (command1));
						memset(command2, 0 , sizeof (command2));
						
						
						printf("Char count: %d\nRead count: %d\nWrite count: %d\n", transferredBytes, readCount, writeCount);
					}
				}					
			}
		
		} else { // input with 1 command: only 1 child
			pid_t n;
			n = fork();	
			
			if (n < 0) { // exception handling
				printf("fork process failed.");
				continue;
			}	
			if (n == 0) { // child process does 

				char path[70];
				//strcpy(path, "/bin/");
				strcat(path, arguments[0]);

				char* args[25];

				int i ;
				
				for (i = 1; i < argumentAmount + 1; i++) {
					args[i] = arguments[i];
				}
				
				args[0] = path;
				args[i] = NULL;

				//execv(path, args);
				if (execvp(args[0], args)) {
					printf("\nError encountered.\n");
					exit(0);
				}
				memset(args, 0, sizeof args);
				memset(arguments, 0,sizeof arguments);

				exit(0);
		
			} else // parent process does
				wait(NULL);

		}
		
		
	}
	

	return 0;
}
