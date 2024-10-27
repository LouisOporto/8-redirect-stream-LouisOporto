#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char* argv[]) {
    char** newargv = (char**)malloc(sizeof(char*) * argc);

    for(int iter = 1; iter < argc; iter++) {
      newargv[iter - 1] = (char*)argv[iter];
    }
    newargv[argc - 1] = NULL;

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