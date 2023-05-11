#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<time.h>
#include<unistd.h>
#include<dirent.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<errno.h>

int main(int argc, char **argv) {
    char name[20];
    strcpy(name, argv[1]);
    mode_t permissions = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP;
    chmod(name, permissions);
}