#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<time.h>
#include<unistd.h>

#define NUMBER_OF_COMMANDS 10

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

void check_access_rights(mode_t mode) {
    printf("User:\n");
    printf("Read - %s\n", ((mode & 0b100000000)!=0) ? "Yes" : "No");
    printf("Write - %s\n", ((mode & 0b10000000)!=0) ? "Yes" : "No");
    printf("Exec - %s\n", ((mode & 0b1000000)!=0) ? "Yes" : "No");
    printf("Group:\n");
    printf("Read - %s\n", ((mode & 0b100000)!=0) ? "Yes" : "No");
    printf("Write - %s\n", ((mode & 0b10000)!=0) ? "Yes" : "No");
    printf("Exec - %s\n", ((mode & 0b1000)!=0) ? "Yes" : "No");
    printf("Others:\n");
    printf("Read - %s\n", ((mode & 0b100)!=0) ? "Yes" : "No");
    printf("Write - %s\n", ((mode & 0b10)!=0) ? "Yes" : "No");
    printf("Exec - %s\n", ((mode & 0b1)!=0) ? "Yes" : "No");
}

void commands_regular_files(char *name, char commands[NUMBER_OF_COMMANDS]) {
    struct stat st;
    stat(name, &st);

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
                printf("Time of last modification: %s\n", ctime(&st.st_mtime));
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
                    printf("Error\n");
                    exit(1);
                }
                break;
            default:
                printf("Unknown command\n");

        }
    }
}

void commands_symbolic_links(char *name, char commands[NUMBER_OF_COMMANDS]) {
    struct stat link, targeted_file;
    lstat(name, &link);
    for(int i=1;i<strlen(commands);i++) {
        switch(commands[i]) {
            case 'n':
                printf("Name of the symbolic link: %s\n", name);
                break;
            case 'l':
                printf("Delete the symbolic link\n");
                unlink(name);
                break;
            case 'd':
                printf("Size of the symbolic link: %ld\n", link.st_size);
                break;
            case 't':
                stat(name, &targeted_file);
                printf("Size of the targeted file: %ld\n", targeted_file.st_size);
                break;
            case 'a':
                check_access_rights(link.st_mode);
                break;
            default:
                printf("Unknown command\n");
        }
        if(commands[i]=='l' && i<strlen(commands)-1) {
            printf("The rest of the commands will not be performed\n");
            break;
        }
    }
}

char* get_commands() {
    char *commands;
    commands = (char*) malloc(NUMBER_OF_COMMANDS*sizeof(char));
    printf("Please insert the desired options: ");
    scanf("%s", commands);

    if(commands[0]!='-') {
        printf("Wrong input\n");
        exit(1);
    }

    return commands;
}

void check_type(char name[]) {
    char commands[NUMBER_OF_COMMANDS];
    struct stat st;

    lstat(name, &st);
    if(S_ISREG(st.st_mode)==0) {
        printf("%s - REGULAR FILE\n", name);
        menu_regular_files();
        strcpy(commands, get_commands());
        commands_regular_files(name, commands);
        return;
    else if(S_ISLNK(st.st_mode)) {
        printf("%s - SYMBOLIC LINK\n", name);
        menu_symbolic_link();
        strcpy(commands, get_commands());
        commands_symbolic_links(name, commands);
        return;
    }
    else if(S_ISDIR(st.st_mode)) {
        printf("%s - DIRECTORY\n", name);
        return;
    }
    else {
        printf("%s - UNKNOWN\n", name);
        return;
    }
}

int main(int argc, char **argv) {
    if(argc==1) {
        printf("Too few arguments\n");
        exit(1);
    }

    struct stat st;

    for(int i=1;i<argc;i++) {
        check_type(argv[i]);
    }

    return 0;
}