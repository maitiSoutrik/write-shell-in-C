#include "commands.h"

// Function to tokenize the command string
char** tokenizeCommand(char *command, int *token_count) {
    char **tokens = malloc(MAX_TOKENS * sizeof(char*));
    char *token;
    *token_count = 0;

    if (!tokens) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(command, DELIMITERS);
    while (token != NULL && *token_count < MAX_TOKENS - 1) {
        tokens[*token_count] = strdup(token);
        if (!tokens[*token_count]) {
            fprintf(stderr, "Allocation error\n");
            exit(EXIT_FAILURE);
        }
        (*token_count)++;
        token = strtok(NULL, DELIMITERS);
    }
    tokens[*token_count] = NULL;
    return tokens;
}

// Function to check and handle output redirection
FILE* checkRedirection(char **tokens, int *token_count) {
    FILE *output = stdout;
    
    // Look for redirection operator
    for (int i = 0; i < *token_count - 1; i++) {
        if (strcmp(tokens[i], ">") == 0) {
            // Open the file for writing
            output = fopen(tokens[i + 1], "w");
            if (!output) {
                fprintf(stderr, "Error: Cannot open file %s\n", tokens[i + 1]);
                return stdout;
            }
            
            // Remove redirection operator and filename from tokens
            free(tokens[i]);
            free(tokens[i + 1]);
            for (int j = i; j < *token_count - 2; j++) {
                tokens[j] = tokens[j + 2];
            }
            *token_count -= 2;
            tokens[*token_count] = NULL;
            break;
        }
    }
    return output;
}

// Function to free allocated tokens
void freeTokens(char **tokens, int token_count) {
    for (int i = 0; i < token_count; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

void executeCommands(char *command)
{
    int token_count;
    char **tokens = tokenizeCommand(command, &token_count);
    FILE *output = checkRedirection(tokens, &token_count);
    
    if (token_count == 0) {
        freeTokens(tokens, token_count);
        if (output != stdout) fclose(output);
        return;
    }

    // Compare command with implemented commands
    if (strcmp(tokens[0], "exit") == 0) {
        fprintf(output, "Goodbye! Shell is closing...\n");
        freeTokens(tokens, token_count);
        if (output != stdout) fclose(output);
        exit(0);
    }
    else if (strcmp(tokens[0], "clear") == 0) {
        // Clear only works on stdout
        if (output == stdout) {
            fprintf(output, "\033[H\033[J");
        }
    }
    else {
        fprintf(output, "Error: Command '%s' not found\n", tokens[0]);
    }

    freeTokens(tokens, token_count);
    if (output != stdout) {
        fclose(output);
    }
}
