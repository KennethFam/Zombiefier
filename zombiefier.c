#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

bool parseArguments(int argc, char* argv[], int* num) {
    bool num_command = false; 
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) num_command = true;
        else if (num_command) {
            for (int j = 0; j < strlen(argv[i]); j++) {
                if (argv[i][j] < 48 || argv[i][j] > 57) {
                    printf("Please enter your numbers argument in the form [-n num] without the brackets. It looks like your num has (a) non-digit character(s) in it.\n");
                    return true;
                }
            }
            *num = atoi(argv[i]);
            if (*num == 0) {
                printf("Please enter a number greater than 0.\n");
                return true;
            }
            break;
        }
        else continue;
    }
    if (num_command) return false;
    else return true;
}

void error_handler() {
    printf("An error has occurred. Please check the messages above for more detail.\n");
}

int main(int argc, char* argv[]) {
    int num;
    if (parseArguments(argc, argv, &num)) {
        error_handler();
        return 1;
    }
    pid_t pid;
    for (int i = 0; i < num; i++) {
        if ((pid = fork()) == 0) {
            exit(0);
        }
        if (pid < 0) {
            fprintf(stderr, "fork function call failed! Error #%d: %s\n", errno, strerror(errno));
            error_handler();
            return 1;
        }
    }
    if (pid) {
        if (kill(getpid(), SIGSTOP) < 0) {
            fprintf(stderr, "kill function call faile! Error #%d: %s\n", errno, strerror(errno));
            error_handler();
            return 1;
        }
    }
    int w_result;
    for (int i = 0; i < num; i++) {
        if(wait(&w_result) < 0) {
            fprintf(stderr, "wait function call failed! Error #%d: %s\n", errno, strerror(errno));
            error_handler();
            return 1;
        }
        if (w_result < 0) {
            printf("One of the child processes exited with an error!\n");
            error_handler();
            return 1;
        }
    }
    return 0;
}
