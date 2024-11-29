#include "commands.h"

// Add  Global variables here

void executeCommands(char *command)
{
    // Compare command with implemented commands
    if (strcmp(command, "exit") == 0) {
        printf("Goodbye! Shell is closing...\n");
        exit(0);  // Exit with success status
    }
    else if (strcmp(command, "clear") == 0) {
        // Use ANSI escape sequence to clear screen and move cursor to top
        printf("\033[H\033[J");  // \033[H moves cursor to top, \033[J clears from cursor to end
    }
    else {
        printf("Error: Command '%s' not found\n", command);
    }
}
