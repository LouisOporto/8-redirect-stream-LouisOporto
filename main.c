#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Usage: redir <inp> <cmd> <out>*/
//
int main(int argc, char* argv[]) {
    if(argc < 4) {
      fprintf(stderr, "Usage: %s <inp> <cmd> <out>\n", argv[0]);
      return 1;
    }
    
    int inputFD = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if(inputFD == -1) {
      fprintf(stderr, "Failed to open %s\n", argv[1]);
      return 1;
    }

    int outputFD = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if(outputFD == -1) {
      fprintf(stderr, "Failed to open %s\n", argv[argc - 1]);
      return 1;
    }

    char** newargv = (char**)malloc(sizeof(char*) * (argc - 2)); // exclude inp, output

    for(int iter = 2; iter < argc; iter++) {
      newargv[iter - 2] = (char*)argv[iter];
    }
    newargv[argc - 2 - 1] = NULL;

    int child_pid = fork();
    if(child_pid == 0) {
      execve(newargv[0], newargv, NULL);
      printf("Error: execve failed\n");
      _exit(1);
    }

    printf("%s pid is %d. forked %d. "
           "Parent exiting\n", argv[0], getpid(), child_pid);
    return 0;
}