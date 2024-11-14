#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define N 12

extern char **environ;

char *allowed[N] = {"cp","touch","mkdir","ls","pwd","cat","grep","chmod","diff","cd","exit","help"};


int isAllowed(const char*cmd) {
	// TODO
	// return 1 if cmd is one of the allowed commands
	// return 0 otherwise
	for (int i = 0; i < N; i++){
		if (strcmp(cmd, allowed[i]) == 0){
			//if the two strings match using string compare, return 1(true)
			return 1;
		}
	}
	
	return 0;
}

int main() {

    // TODO
    // Add variables as needed

    char line[256];

    while (1) {

	fprintf(stderr,"rsh>");

	if (fgets(line,256,stdin)==NULL) continue;

	if (strcmp(line,"\n")==0) continue;

	line[strlen(line)-1]='\0';

	// TODO
	// Add code to spawn processes for the first 9 commands
	// And add code to execute cd, exit, help commands
	// Use the example provided in myspawn.c

	//tokenize input in order to separate command and arguments
	char *argv[21]; // array to hold command and args
	int argc = 0; //arg count
	char *token = strtok(line, " "); //tokenize input
	while (token != NULL && argc <20){
		argv[argc++] = token;
		token = strtok(NULL, " ");

	}
	argv[argc] = NULL; //null-terminate argv array
	//if there is empty input (no command) skip the iteration
	if (argc == 0) continue; //empty input

	//check if command is allowed
	if(!isAllowed(argv[0])) {
		 //enters this if statement if a 0 is returned from the isAllowed function
		 printf("NOT ALLOWED! \n");
		 continue;	    
	 }
	
	//if/else if statements to deal with specific commands that do not require spawing a new process 
	if (strcmp(argv[0], "cd") == 0) {
		//too many arguments for cd
		if(argc > 2) {
			printf("-rsh: cd: too many arguments\n");
		} else if (argc ==2 && chdir(argv[1]) !=0) {
			//print error
			perror("rsh cd failed");
		}
	} else if (strcmp(argv[0], "exit") == 0) {
		//exits the shell
		    return 0;
	} else if (strcmp(argv[0], "help") == 0) {
		//lists allowed commands
		printf("The allowed commands are: \n");
		for(int i = 0; i < N; i++) {
			printf("%d: %s\n", i+1, allowed[i]);
		}
	} else if (strcmp(argv[0], "mkdir") == 0) {
		//checks if no argument is given for mkdir
		if(argc < 2) {
			printf("rsh: mkdir: missing operand for mkdir \n");
		} else {
			//create directories with 0777 permissions for each arg
			for(int i = 1; i < argc; i++) {
				if (mkdir(argv[i], 0777) != 0){
					perror("rsh mkdir failed");
				}  
			}
		    }
	} else if (strcmp(argv[0], "rmdir") == 0){
		//rmdir is now allowed 
		printf("NOT ALLOWED! \n");
	} else if (strcmp(argv[0], "touch") == 0){
		//check if no arg is given
		if(argc < 2){
			printf("rsh: touch: missing operand\n");
		} else {
			//create file with write permissions
			for (int i = 1; i < argc; i++){
				int fd = open(argv[i], O_CREAT | O_WRONLY, 0644);
				if (fd == -1){
					perror("rsh touch failed");
				} else {
					close(fd);	
				}	
			}	
		}
	} else if (strcmp(argv[0], "ls") == 0) {
		//ls is a spawned process
		pid_t pid;
		int status;
		posix_spawnattr_t attr;
		posix_spawnattr_init(&attr);

		if(posix_spawnp(&pid, "ls" , NULL, &attr, argv, environ) !=0){
			perror("spawn failed :(");
		}

		if(waitpid(pid, &status, 0) == -1) {
			perror("waitpid failed");
		}
	} else {
		//for commands not explicitly handled above, spawn the process and execute command
		pid_t pid;
		int status;
		posix_spawnattr_t attr;
		posix_spawnattr_init(&attr);

		if(posix_spawnp(&pid, argv[0], NULL, &attr, argv, environ) !=0){
			perror("spawn failed :(");
		}

		//wait for spawned process to end
		if(waitpid(pid, &status, 0) == -1) {
			perror("waitpid failed");
		}
	}

    }
    return 0; //end main loop and program exits
}
