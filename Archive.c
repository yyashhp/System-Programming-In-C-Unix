#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Must Follow Executable by A File Name To Check: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct stat sb;
    if (stat(argv[1], &sb) == -1) {
        perror("stat error");
        exit(EXIT_FAILURE);
    }

    time_t now = time(NULL);
    if (now - sb.st_mtime > 4 * 7 * 24 * 60 * 60) { // 4 weeks in format
        if (chmod(argv[1], sb.st_mode & ~(S_IWUSR | S_IWGRP | S_IWOTH)) == -1) { // remove write permissions for owner, group, and others using the bitwise 
//complement operator
            perror("chmod error");
            exit(EXIT_FAILURE);
        }
        printf("File %s has been set to read-only\n", argv[1]);
    } else {
        printf("File %s has been modified within the last 4 weeks\n", argv[1]);
    }

    return(EXIT_SUCCESS);
}

