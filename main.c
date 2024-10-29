#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

//
/* Usage: redir <inp> <cmd> <out>*/
//

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <inp> <cmd> <out>\n", argv[0]);
        return 1;
    }

    int inputFD;
    if (strcmp(argv[1], "-") == 0) {
        printf("default input\n");
    } else {
        inputFD = open(argv[1], O_RDONLY, S_IRUSR);
        if (inputFD == -1) {
            fprintf(stderr, "Failed to open %s\n", argv[1]);
            return 1;
        }
    }

    int outputFD;
    if (strcmp(argv[3], "-") == 0) {
        printf("default output\n");
    } else {
        outputFD = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC,
                        S_IWUSR | S_IRUSR);
        if (outputFD == -1) {
            fprintf(stderr, "Failed to open %s\n", argv[argc - 1]);
            return 1;
        }
    }

    // Copy arguments
    char** newargv = (char**)malloc(sizeof(char*) * (argc - 2));
    char prefix[] = "/usr/bin/";
    size_t cmdLength = strlen(prefix) + strlen(argv[2]) + 1;
    char* cmdPath = (char*)malloc(cmdLength);

    snprintf(cmdPath, cmdLength, "%s%s", prefix, argv[2]);
    // printf("%s\n", cmdPath);

    newargv[0] = cmdPath;
    for (int iter = 3; iter < argc - 1; iter++) {
        newargv[iter - 2] = (char*)argv[iter];
    }
    newargv[argc - 3] = NULL;

    // Fork and execute
    int child_pid = fork();
    if (child_pid == 0) {
        if (strcmp(argv[1], "-") != 0) {
            printf("test");
            dup2(inputFD, STDIN_FILENO);
            close(inputFD);
        }
        if (strcmp(argv[3], "-") != 0) {
            dup2(outputFD, STDOUT_FILENO);
            close(outputFD);
        }

        execve(newargv[0], newargv, NULL);
        printf("Error: execve failed\n");
        _exit(1);
    }

    printf(
        "%s pid is %d. forked %d. "
        "Parent exiting\n",
        argv[0], getpid(), child_pid);
    return 0;
}