#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PATH "/usr/bin/"

//
/* Usage: redir <inp> <cmd> <out>*/
//

// CREATE a path file to the commands in /usr/bin/ function


int main(int argc, char* argv[]) {
    int fds[2];
    pipe(fds);
    pid_t pid = fork();

    if (pid == 0) {
      dup2(fds[1], STDIN_FILENO);
      close(fds[0]);
      close(fds[1]);
      if(execve(argv[0], argv, NULL) == -1) { exit(0); }
    }

    close(fds[0]);
    const char* input[] = {"echo", "-l", "tree", NULL};
    for(int i = 0; input[i] != NULL; i++) {
      dprintf(fds[1], "%s\n", input[i]);
    }

    close(fds[1]);
    int status;
    pid_t wpid = waitpid(pid, &status, 0);
    return wpid == pid && WIFEEXITED(status) ? WEXITSTATUS(status) : -1;
    // if(argc != 4) {
    //   fprintf(stderr, "Usage: %s <inp> <cmd> <out>\n", argv[0]);
    //   return 1;
    // }
    
    // int inputFD = open(argv[1], O_RDONLY, S_IRUSR);
    // if(inputFD == -1) {
    //   fprintf(stderr, "Failed to open %s\n", argv[1]);
    //   return 1;
    // }

    // int outputFD = open(argv[argc - 1], O_RDWR | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR);
    // if(outputFD == -1) {
    //   fprintf(stderr, "Failed to open %s\n", argv[argc - 1]);
    //   return 1;
    // }

    // // Need to read the input file and write each command as separated parameter
    // // to the newargv array
    // char** newargv = (char**)malloc(sizeof(char*) * (argc - 2)); // change to number of elements + cmd selected


    // // REMOVE
    // for(int iter = 2; iter < argc; iter++) {
    //   newargv[iter - 2] = (char*)argv[iter];
    // }
    // newargv[argc - 2 - 1] = NULL;

    

    // // Fork and execute
    // int child_pid = fork();
    // if(child_pid == 0) {
    //   execve(newargv[0], newargv, NULL);
    //   printf("Error: execve failed\n");
    //   _exit(1);
    // }

    // printf("%s pid is %d. forked %d. "
    //        "Parent exiting\n", argv[0], getpid(), child_pid);
    // return 0;
}