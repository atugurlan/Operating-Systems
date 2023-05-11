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

#define NUMBER_OF_COMMANDS 10

void reset_commands(char *name);

void check_correctness_commands(char commands[], char name[]);

//print the menu for the 3 types of inputs
void menu_regular_files() {
    printf("--- MENU ---\n");
    printf("Name: -n\n");
    printf("Size: -d\n");
    printf("Hard link count: -h\n");
    printf("Time of last modification: -m\n");
    printf("Access rights: -a\n");
    printf("Create symbolic link: -l\n");
}

void menu_symbolic_link() {
    printf("--- MENU ---\n");
    printf("Name: -n\n");
    printf("Delete symbolic link: -l\n");
    printf("Size of symbolic link: -d\n");
    printf("Size of targeted file: -t\n");
    printf("Access rights: -a\n");
}

void menu_directories() {
    printf("---MENU---\n");
    printf("Name: -n\n");
    printf("Size: -d\n");
    printf("Access rights: -a\n");
    printf("Count how many c files there are in the directory: -c\n");
}

//get the access rights
void check_access_rights(mode_t mode) {
    printf("User:\n");
    printf("Read - %s\n", ((mode & S_IRUSR)!=0) ? "Yes" : "No");
    printf("Write - %s\n", ((mode & S_IWUSR)!=0) ? "Yes" : "No");
    printf("Exec - %s\n", ((mode & S_IXUSR)!=0) ? "Yes" : "No");
    printf("Group:\n");
    printf("Read - %s\n", ((mode & S_IRGRP)!=0) ? "Yes" : "No");
    printf("Write - %s\n", ((mode & S_IWGRP)!=0) ? "Yes" : "No");
    printf("Exec - %s\n", ((mode & S_IXGRP)!=0) ? "Yes" : "No");
    printf("Others:\n");
    printf("Read - %s\n", ((mode & S_IROTH)!=0) ? "Yes" : "No");
    printf("Write - %s\n", ((mode & S_IWOTH)!=0) ? "Yes" : "No");
    printf("Exec - %s\n", ((mode & S_IXOTH)!=0) ? "Yes" : "No");
}

// regular file commands
void c_file(char name[]) {
    if(name[strlen(name)-1]=='c' && name[strlen(name)-2]=='.') {
        int pfd[2];
        pid_t pid2;

        if(pipe(pfd)<0) {
            perror(strerror(errno));
            exit(errno);
        }

        if((pid2=fork())<0) {
            perror("Child process creation failed\n");
            exit(1);
        }
        else if(pid2==0) {
            close(pfd[0]);
            dup2(pfd[1], 1);

            int check = execlp("bash", "bash", "compile.sh", name, NULL);
            if(check==-1) {
                printf("Error calling exec\n");
                exit(1);
            }
        }
        else {
           int wstatus1;
            pid_t w1;
            w1 = wait(&wstatus1);
            if(WIFEXITED(wstatus1)) {
                printf("The process with PID <%d> has ended with the exit code <%d>\n", w1, WEXITSTATUS(wstatus1));
            }
            int wstatus2;
            pid_t w2;
            w2 = wait(&wstatus2);
            if(WIFEXITED(wstatus2)) {
                printf("The process with PID <%d> has ended with the exit code <%d>\n", w2, WEXITSTATUS(wstatus2));
            }
            close(pfd[1]);

            int no_errors;
            int no_warnings;

            char buffer[100];
            int check = read(pfd[0], buffer, sizeof(buffer));
            if(check==-1) {
                printf("error at read()");
                exit(1);
            }

            char *token;
            token = strtok(buffer, " ");
            int counter = 0;
            while(token != NULL) {
                if(counter == 0) {
                    no_errors = atoi(token);
                }
                else {
                    no_warnings = atoi(token);
                }
                token = strtok(NULL, " ");
                counter++;
            }

            close(pfd[0]);

            int score;

            if(no_errors == 0) {
                if(no_warnings == 0) {
                    score = 10;
                }
                else if(no_warnings > 10) {
                    score = 2;
                }
                else {
                    score = 2 + 8 * (10 - no_warnings) / 10;
                }
            } 
            else if(no_errors >= 1) {
                score = 1;
            }

            int fd = open("grades.txt", O_RDWR);
            if(fd == -1) {
                printf("error for fd()");
                exit(3);
            }

            char score_text[3];

            score_text[0] = score/10 + '0';
            score_text[1] = score%10 + '0';
            score_text[2] = '\0';

            char file_text[100];
            strcpy(file_text, name);
            strcat(file_text, " : ");
            strcat(file_text, score_text);

            int wrote = write(fd, file_text, strlen(file_text));
            if(wrote == -1) {
                printf("error at writting\n");
                exit(3);
            }
            close(fd);
        }
    }
    else {
        pid_t pid2;
        pid2 = fork();

        if(pid2<0) {
            printf("error at fork() 2\n");
            exit(1);
        }
        else if(pid2==0) {
            int check = execlp("wc", "wc", "-l", name, NULL);
            if(check==-1) {
                printf("error at execlp");
                exit(1);
            }
        }
        else {
            int wstatus1;
            pid_t w1;
            w1 = wait(&wstatus1);
            if(WIFEXITED(wstatus1)) {
                printf("The process with PID <%d> has ended with the exit code <%d>\n", w1, WEXITSTATUS(wstatus1));
            }
            int wstatus2;
            pid_t w2;
            w2 = wait(&wstatus2);
            if(WIFEXITED(wstatus2)) {
                printf("The process with PID <%d> has ended with the exit code <%d>\n", w2, WEXITSTATUS(wstatus2));
            }
        }
    }
    
}

void commands_regular_files(char name[], char commands[NUMBER_OF_COMMANDS]) {
    check_correctness_commands(commands, name);

    char letters[6] = "ndhmal";
    for(int i=1;i<strlen(commands);i++) {
        if(strchr(letters, commands[i]) == NULL) {
            printf("You entered a command that is not in the commands menu\n");
            reset_commands(name);
        }
    }

    struct stat st;
    if(stat(name, &st)==-1) {
        printf("Error stat() for regular file in commands_regular_files\n");
        exit(1);
    }

    for(int i=1;i<strlen(commands);i++) {
        switch(commands[i]) {
            case 'n': 
                printf("Name of the file: %s\n", name);
                break;
            case 'd': 
                printf("Size: %ld\n", st.st_size);
                break;
            case 'h': 
                printf("Hard link count: %ld\n", st.st_nlink);
                break;
            case 'm':
                printf("Time of last modification: %s", ctime(&st.st_mtime));
                break;
            case 'a': 
                check_access_rights(st.st_mode);
                break;
            case 'l': 
                printf("Please enter the name of the link: ");
                char link_name[20];
                scanf("%s", link_name);
                if(symlink(name, link_name)==0) {
                    printf("Link created succesfully\n");
                }
                else {
                    printf("Error creating the symlink\n");
                    exit(1);
                }
                break;
        }
    }
}

// symbolic link commands
void change_permissions(char name[]) {
    pid_t pid2;
    pid2 = fork();

    if(pid2<0) {
        printf("error at fork()\n");
        exit(1);
    }
    else if(pid2==0) {
        // mode_t permissions = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP;
        // int changed = chmod(name, permissions);

        int changed = execlp("chmod", "chmod", "u+rwx,g+rw-x,o-rwx", name, NULL);

        if(changed == -1) {
            printf("error at changing permissions");
            exit(3);
        }
    }
    else {
        int wstatus1;
            pid_t w1;
            w1 = wait(&wstatus1);
            if(WIFEXITED(wstatus1)) {
                printf("The process with PID <%d> has ended with the exit code <%d>\n", w1, WEXITSTATUS(wstatus1));
            }
            int wstatus2;
            pid_t w2;
            w2 = wait(&wstatus2);
            if(WIFEXITED(wstatus2)) {
                printf("The process with PID <%d> has ended with the exit code <%d>\n", w2, WEXITSTATUS(wstatus2));
            }
    }
}

void commands_symbolic_links(char *name, char commands[NUMBER_OF_COMMANDS]) {
    check_correctness_commands(commands, name);

    char letters[5] = "nldta";
    for(int i=1;i<strlen(commands);i++) {
        if(strchr(letters, commands[i]) == NULL) {
            printf("You entered a command that is not in the commands menu\n");
            reset_commands(name);
        }
    }

    struct stat link, targeted_file;
    if(lstat(name, &link)==-1) {
        printf("Error lstat() for symlink in commands_symbolic_links\n");
        exit(1);
    }

    for(int i=1;i<strlen(commands);i++) {
        switch(commands[i]) {
            case 'n':
                printf("Name of the symbolic link: %s\n", name);
                break;
            case 'l':
                if(unlink(name)==-1) {
                        printf("error unlink() in commands_symbolic_links\n");
                        exit(1);
                }
                else {
                    printf("Successfully delete the symbolic link\n");
                }
                break;
            case 'd':
                printf("Size of the symbolic link: %ld\n", link.st_size);
                break;
            case 't':
                if(stat(name, &targeted_file)==-1) {
                    printf("error stat() for targeted file in commands_symbolic_links\n)");
                    exit(1);
                }
                printf("Size of the targeted file: %ld\n", targeted_file.st_size);
                break;
            case 'a':
                check_access_rights(link.st_mode);
                break;
        }     
        if(commands[i]=='l' && i<strlen(commands)-1) {
            printf("The rest of the commands will not be performed, because the link was deleted\n");
            break;
        }
    }
}

// directory commands
void create_new_file(char name[]) {
    pid_t pid2;
    pid2 = fork();
 
    if(pid2<0) {
        printf("error at fork()\n");
        exit(1);
    }
    else if(pid2==0) {
        char file_name[100];
        char path[150];

        strcpy(file_name, name);
        strcat(file_name, "_file.txt");

        strcpy(path, name);
        strcat(path, "/");
        strcat(path, file_name);

        int created = creat(path, S_IRUSR);
        if(created == -1) {
            printf("error at creating file\n");
            exit(2);
        }
        close(created);
    }
    else {
        int wstatus1;
        pid_t w1;
        w1 = wait(&wstatus1);
        if(WIFEXITED(wstatus1)) {
            printf("The process with PID <%d> has ended with the exit code <%d>\n", w1, WEXITSTATUS(wstatus1));
        }
            int wstatus2;
            pid_t w2;
            w2 = wait(&wstatus2);
            if(WIFEXITED(wstatus2)) {
                printf("The process with PID <%d> has ended with the exit code <%d>\n", w2, WEXITSTATUS(wstatus2));
            }
    }
}

void commands_directory(char *name, char commands[]) {
    check_correctness_commands(commands, name);

    char letters[4] = "ndac";
    for(int i=1;i<strlen(commands);i++) {
        if(strchr(letters, commands[i]) == NULL) {
            printf("You entered a command that is not in the commands menu\n");
            reset_commands(name);
        }
    }

    DIR *dir;
    struct dirent *entry;
    dir = opendir(name);

    if(dir==NULL) {
        printf("Error at opening directory\n");
        exit(1);
    }

    struct stat st;
    lstat(name, &st);

    for(int i=1;i<strlen(commands);i++) {
        switch(commands[i]) {
            case 'n':
                printf("Name of the directory: %s\n", name);
                break;
            case 'd':
                printf("The size of the directory: %ld\n", st.st_size);
                break;
            case 'a':
                check_access_rights(st.st_mode);
                break;
            case 'c':
                int count = 0;

                while( (entry = readdir(dir)) != NULL ) {
                    if( entry->d_name[strlen(entry->d_name)-2]=='.' && entry->d_name[strlen(entry->d_name)-1]=='c'  )
                        count++;
                }

                printf("Number of C files: %d\n", count);
                break;
            default:
                printf("You entered a command that is not in the commands menu\n");
                reset_commands(name);
        }
    }

    closedir(dir);
}

char* get_commands() {
    char *commands;
    commands = (char*) malloc(NUMBER_OF_COMMANDS*sizeof(char));
    if(commands==NULL) {
        printf("Error malloc() for the commands string in get_commands\n");
        exit(1);
    }
    printf("Please insert the desired options: ");
    scanf("%s", commands);

    return commands;
}

//go to the designated type
void execute_commands_for_regular_file(char name[]) {
    char commands[NUMBER_OF_COMMANDS];

    menu_regular_files();
    strcpy(commands, get_commands());
    commands_regular_files(name, commands);
}

void execute_commands_for_symbolic_link(char name[]) {
    char commands[NUMBER_OF_COMMANDS];

    menu_symbolic_link();
    strcpy(commands, get_commands());
    commands_symbolic_links(name, commands);
}

void execute_commands_for_directory(char name[]) {
    char commands[NUMBER_OF_COMMANDS];

    menu_directories();
    strcpy(commands, get_commands());
    commands_directory(name, commands);
}

int main(int argc, char **argv) {
    if(argc==1) {
        printf("Too few arguments\n");
        exit(1);
    }

    char path[50];
    char name[50];

    for(int i=1;i<argc;i++) {
        strcpy(path, argv[i]);
        
        int i = strlen(path) - 1;
        while(i>=0 && path[i]!='/') {
            i--;
        }
        i = i + 1;

        name[0] = '\0';
        int j=0;
        while(i<strlen(path)) {
            name[j] = path[i];
            j++;
            i++;
        }  
        name[j] = '\0';

        struct stat st;

        if(lstat(name, &st)==-1) {
            printf("Error lstat() in function check_type\n");
            exit(1);
        }

        pid_t pid1;
        pid1 = fork();

        if(pid1 < 0) {
            printf("error at fork() for child 1\n");
            exit(1);
        }
        else if(pid1 == 0) {
            if(S_ISREG(st.st_mode)) {
                printf("%s - REGULAR FILE\n", name);
                execute_commands_for_regular_file(name);
            }
            else if(S_ISLNK(st.st_mode)) {
                printf("%s - SYMBOLIC LINK\n", name);
                execute_commands_for_symbolic_link(name);
            }
            else if(S_ISDIR(st.st_mode)) {
                printf("%s - DIRECTORY\n", name);
                execute_commands_for_directory(name);
            }
            else {
                printf("%s - UNKNOWN\n", name);
                exit(1);
            }
        }
        else {
            if(S_ISREG(st.st_mode)) {
                c_file(name);
            }
            else if(S_ISLNK(st.st_mode)) {
                change_permissions(name);
            }
            else if(S_ISDIR(st.st_mode)) {
                create_new_file(name);
            }

            // Tried:
            // int wstatus;
            // pid_t w;
            // w = wait(&wstatus);
            // if(WIFEXITED(wstatus)) {
            //     printf("The process with PID <%d> has ended with the exit code <%d>\n", w, WEXITSTATUS(wstatus));
            // }
        }
    }

    return 0;
}

void reset_commands(char name[]) {
    char commands[NUMBER_OF_COMMANDS];

    printf("\n\nRenter the commands:\n");

    struct stat st;

    if(lstat(name, &st)==-1) {
        printf("Error lstat() in function check_type\n");
        exit(1);
    }

    if(S_ISREG(st.st_mode)) {
        menu_regular_files();
        strcpy(commands, get_commands());
        commands_regular_files(name, commands);
    }
    else if(S_ISLNK(st.st_mode)) {
        menu_symbolic_link();
        strcpy(commands, get_commands());
        commands_symbolic_links(name, commands);
    }
    else if(S_ISDIR(st.st_mode)) {
        menu_directories();
        strcpy(commands, get_commands());
        commands_directory(name, commands);
    }
}

void check_correctness_commands(char commands[], char name[]) {
    if(commands[0]!='-') {
        printf("\n\nThe commands you entered are in the wrong format\n");
        printf("Please enter the commands instructions as follow: -xyzetc, where x, y, z etc are the commands in questions\n");
        reset_commands(name);
    }
}

