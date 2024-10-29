#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define PATH "/usr/bin/"

//
/* Usage: redir <inp> <cmd> <out>*/
//

// CREATE a path file to the commands in /usr/bin/ function


int main(int argc, char* argv[]) {
    if(argc < 4) {
      fprintf(stderr, "Usage: %s <inp> <cmd> <out>\n", argv[0]);
      return 1;
    }
    
    int inputFD;
    if(strcmp(argv[1], "-") == 0) {
      printf("default input\n");
    }
    else {
      inputFD = open(argv[1], O_RDONLY, S_IRUSR);
      if(inputFD == -1) {
        fprintf(stderr, "Failed to open %s\n", argv[1]);
        return 1;
      }
    }

    int outputFD;
    if(strcmp(argv[3], "-") == 0) {
      printf("default output\n");
    }
    else {
      outputFD = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR);
      if(outputFD == -1) {
        fprintf(stderr, "Failed to open %s\n", argv[argc - 1]);
        return 1;
      }
    }

    // Need to read the input file and write each command as separated parameter
    // to the newargv array
    char** newargv = (char**)malloc(sizeof(char*) * (argc - 2)); // change to number of elements + cmd selected


    // // REMOVE
    for(int iter = 2; iter < argc - 1; iter++) {
      newargv[iter - 2] = (char*)argv[iter];
    }
    newargv[argc - 3] = NULL;


    // Fork and execute
    int child_pid = fork();
    if(child_pid == 0) {
      dup2(inputFD, STDIN_FILENO);
      close(inputFD);
      dup2(outputFD, STDOUT_FILENO);
      close(outputFD);

      execve(newargv[0], newargv, NULL);
      printf("Error: execve failed\n");
      _exit(1);
    }


    printf("%s pid is %d. forked %d. "
           "Parent exiting\n", argv[0], getpid(), child_pid);
    return 0;
}