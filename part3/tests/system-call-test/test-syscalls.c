#include <stdio.h>
#include "test-syscalls.h"

int main() {
    if(start_elevator() == 0)
        printf("start_elevator system call exists.\n");
    else
        printf("stop_elevator system call does not exist.\n");

    if(issue_request(1, 2, 3) == 0)
        printf("issue_request system call exists.\n");
    else
        printf("issue_request system call does not exist.\n");

    if(stop_elevator() == 0)
        printf("stop_elevator system call exists.\n");
    else
        printf("stop_elevator system call does not exist.\n");

    return 0;
}