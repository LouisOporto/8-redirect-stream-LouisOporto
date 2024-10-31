#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

//
/* Usage: redir <inp> <cmd> <out>*/
//

void addCharacterToString(char* str, char c) {
    int leng = strlen(str);
    str[leng] = c;
    str[leng + 1] = '\0';
}

// Split string by spaces; adds a NULL into the array after the last word
void split(char* cmd, char*words[], char delimiter) {
    int wordCount = 0;
    char* nextChar = cmd;
    char currentWord[10000];
    strcpy(currentWord, "");

    while(*nextChar != '\0') {
        if(*nextChar == delimiter) {
            words[wordCount++] = strdup(currentWord);
            strcpy(currentWord, "");
        } else {
            addCharacterToString(currentWord, *nextChar);
        }
        // printf("next_char = '%c'\n", *nextChar);
        ++nextChar;
    }

    words[wordCount++] = strdup(currentWord);  // The loop will not add this since it only checks for spaces and not EOF
    words[wordCount] = NULL;
}

bool findAbsolutePath(char* cmd, char* absolutePath) {
    strcpy(absolutePath, cmd);
    char *dir[1000];
    split(getenv("PATH"), dir, ':');

    for(int ix = 0; dir[ix] != NULL; ix++) {
        char path[1000];
        strcpy(path, dir[ix]);
        addCharacterToString(path, '/');
        strcat(path, cmd);

        if(access(path, F_OK) == 0) {
            strcpy(absolutePath, path);
            return true;
        }
    }
    return false;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
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
    // char** newargv = (char**)malloc(sizeof(char*) * (argc - 2));
    // char prefix[] = "/usr/bin/";
    // size_t cmdLength = strlen(prefix) + strlen(argv[2]) + 1;
    // char* cmdPath = (char*)malloc(cmdLength);

    // snprintf(cmdPath, cmdLength, "%s%s", prefix, argv[2]);
    // // printf("%s\n", cmdPath);

    // newargv[0] = cmdPath;
    // for (int iter = 3; iter < argc - 1; iter++) {
    //     newargv[iter - 2] = (char*)argv[iter];
    // }
    // newargv[argc - 3] = NULL;

    char absolutePath[1000];
    char *words[1000];

    split(argv[2], words, ' ');

    if(words[0] == NULL) {
        printf("Error: No command specified\n");
        return  1;
    }

    if(!findAbsolutePath(words[0], absolutePath)) {
        printf("Error: Command not found\n");
        return 1;
    }

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

        execve(absolutePath, words, NULL);
        printf("Error: execve failed\n");
        _exit(1);
    }

    printf(
        "%s pid is %d. forked %d. "
        "Parent exiting\n",
        argv[0], getpid(), child_pid);
    return 0;
}