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

int main() {
    for(int i=0;i<2;i++) {
        pid_t pid = fork();
        printf("%d\n", i);
    }
}