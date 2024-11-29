#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>
#include <ctype.h>
#include "commands.h"

#define COLOR_RESET "\033[0m"
#define COLOR_CYAN "\033[0;36m"
#define MAX_INPUT_SIZE 1024
#define MAX_PATH_SIZE 1024

// Function to trim whitespace from both ends of a string
char* trim(char* str) {
    char* end;
    
    // Trim leading whitespace
    while(isspace((unsigned char)*str)) str++;
    
    if(*str == 0) // All spaces
        return str;
        
    // Trim trailing whitespace
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    
    end[1] = '\0';
    
    return str;
}

// Process command code
void processCommand(char* command) {
    // Trim whitespace from the command
    char* trimmed_command = trim(command);
    
    // If command is not empty, execute it
    if(strlen(trimmed_command) > 0) {
        executeCommands(trimmed_command);
    }
}

int main(int argc, char *argv[]) {
    char hostname[MAX_PATH_SIZE];
    char cwd[MAX_PATH_SIZE];
    char input[MAX_INPUT_SIZE];
    struct passwd *pw;
    
    // Get the username
    pw = getpwuid(getuid());
    if (pw == NULL) {
        perror("Error getting username");
        exit(1);
    }
    
    while(1) {
        // Get hostname
        if (gethostname(hostname, sizeof(hostname)) != 0) {
            perror("Error getting hostname");
            exit(1);
        }
        
        // Get current working directory
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("Error getting current directory");
            exit(1);
        }
        
        // Print prompt with username, hostname and current directory
        printf("%s%s@%s:%s%s$ ", COLOR_CYAN, pw->pw_name, hostname, cwd, COLOR_RESET);
        
        // Get input from user
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            if (feof(stdin)) {
                printf("\n");
                break;  // Exit on Ctrl-D
            }
            perror("Error reading input");
            exit(1);
        }
        
        // Remove trailing newline
        input[strcspn(input, "\n")] = 0;
        
        // Process the command
        processCommand(input);
    }
    
    return 0;
}