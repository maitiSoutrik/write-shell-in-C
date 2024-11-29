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

// Helper function to format file permissions
void format_permissions(mode_t mode, char *perms) {
    perms[0] = (S_ISDIR(mode)) ? 'd' : '-';
    perms[1] = (mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (mode & S_IXUSR) ? 'x' : '-';
    perms[4] = (mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (mode & S_IXGRP) ? 'x' : '-';
    perms[7] = (mode & S_IROTH) ? 'r' : '-';
    perms[8] = (mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (mode & S_IXOTH) ? 'x' : '-';
    perms[10] = '\0';
}

// Helper function to format file time
void format_time(time_t time, char *time_str) {
    struct tm *tm_info = localtime(&time);
    strftime(time_str, 20, "%b %d %H:%M", tm_info);
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
    else if (strcmp(tokens[0], "echo") == 0) {
        // Print each argument after "echo", separated by spaces
        for (int i = 1; i < token_count; i++) {
            fprintf(output, "%s", tokens[i]);
            if (i < token_count - 1) {
                fprintf(output, " ");  // Add space between words, but not after last word
            }
        }
        fprintf(output, "\n");  // Add newline at the end
    }
    else if (strcmp(tokens[0], "cat") == 0) {
        if (token_count < 2) {
            fprintf(output, "Error: cat command requires a file argument\n");
        } else {
            // Check if file exists and is a regular file
            struct stat file_stat;
            if (stat(tokens[1], &file_stat) == -1) {
                fprintf(output, "Error: File '%s' does not exist\n", tokens[1]);
            } else if (!S_ISREG(file_stat.st_mode)) {
                fprintf(output, "Error: '%s' is not a regular file\n", tokens[1]);
            } else {
                // Open and read the file
                FILE *file = fopen(tokens[1], "r");
                if (!file) {
                    fprintf(output, "Error: Cannot open file '%s'\n", tokens[1]);
                } else {
                    char buffer[1024];
                    while (fgets(buffer, sizeof(buffer), file) != NULL) {
                        fprintf(output, "%s", buffer);
                    }
                    fclose(file);
                }
            }
        }
    }
    else if (strcmp(tokens[0], "pwd") == 0) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            fprintf(output, "%s\n", cwd);
        } else {
            fprintf(output, "Error: Could not get current directory\n");
        }
    }
    else if (strcmp(tokens[0], "cd") == 0) {
        const char *dir;
        if (token_count < 2) {
            // No argument provided, change to home directory
            dir = "/Users/soutrikmaiti/Documents/git_repos/write-shell-in-C";
        } else {
            dir = tokens[1];
        }
        
        if (chdir(dir) != 0) {
            fprintf(output, "Error: Could not change directory to '%s': %s\n", 
                    dir, strerror(errno));
        }
    }
    else if (strcmp(tokens[0], "ls") == 0) {
        DIR *dir;
        struct dirent *entry;
        int use_long_format = 0;
        const char *target_dir = ".";  // Default to current directory

        // Check for -l flag and directory argument
        for (int i = 1; i < token_count; i++) {
            if (strcmp(tokens[i], "-l") == 0) {
                use_long_format = 1;
            } else {
                target_dir = tokens[i];
            }
        }

        dir = opendir(target_dir);
        if (dir == NULL) {
            fprintf(output, "Error: Could not open directory '%s': %s\n", 
                    target_dir, strerror(errno));
        } else {
            if (use_long_format) {
                // Print total block count
                long total = 0;
                while ((entry = readdir(dir)) != NULL) {
                    struct stat st;
                    char full_path[PATH_MAX];
                    snprintf(full_path, sizeof(full_path), "%s/%s", target_dir, entry->d_name);
                    if (stat(full_path, &st) == 0) {
                        total += st.st_blocks;
                    }
                }
                fprintf(output, "total %ld\n", total / 2);  // Convert to 1K blocks
                rewinddir(dir);

                // Print detailed listing
                while ((entry = readdir(dir)) != NULL) {
                    char full_path[PATH_MAX];
                    struct stat st;
                    snprintf(full_path, sizeof(full_path), "%s/%s", target_dir, entry->d_name);
                    
                    if (stat(full_path, &st) == 0) {
                        char perms[11];
                        char time_str[20];
                        struct passwd *pw = getpwuid(st.st_uid);
                        struct group *gr = getgrgid(st.st_gid);
                        
                        format_permissions(st.st_mode, perms);
                        format_time(st.st_mtime, time_str);
                        
                        fprintf(output, "%s %3lu %-8s %-8s %8lld %s %s\n",
                                perms,
                                (unsigned long)st.st_nlink,
                                pw ? pw->pw_name : "unknown",
                                gr ? gr->gr_name : "unknown",
                                (long long)st.st_size,
                                time_str,
                                entry->d_name);
                    }
                }
            } else {
                // Simple listing
                while ((entry = readdir(dir)) != NULL) {
                    fprintf(output, "%s  ", entry->d_name);
                }
                fprintf(output, "\n");
            }
            closedir(dir);
        }
    }
    else if (strcmp(tokens[0], "mkdir") == 0) {
        if (token_count < 2) {
            fprintf(output, "Error: mkdir command requires a directory name argument\n");
        } else {
            // Create directory with standard permissions (rwxr-xr-x)
            if (mkdir(tokens[1], 0755) != 0) {
                fprintf(output, "Error: Could not create directory '%s': %s\n",
                        tokens[1], strerror(errno));
            }
        }
    }
    else if (strcmp(tokens[0], "touch") == 0) {
        if (token_count < 2) {
            fprintf(output, "Error: touch command requires a file name argument\n");
        } else {
            // Try to open the file in append mode, create if it doesn't exist
            int fd = open(tokens[1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1) {
                fprintf(output, "Error: Could not create file '%s': %s\n",
                        tokens[1], strerror(errno));
            } else {
                // Update the access and modification times
                struct timespec times[2] = {{0, UTIME_NOW}, {0, UTIME_NOW}};
                if (futimens(fd, times) == -1) {
                    fprintf(output, "Warning: Could not update timestamps for '%s': %s\n",
                            tokens[1], strerror(errno));
                }
                close(fd);
            }
        }
    }
    else if (strcmp(tokens[0], "rm") == 0) {
        if (token_count < 2) {
            fprintf(output, "Error: rm command requires a file or directory name argument\n");
        } else {
            struct stat path_stat;
            
            // Check if path exists and get its type
            if (stat(tokens[1], &path_stat) == -1) {
                fprintf(output, "Error: '%s' does not exist\n", tokens[1]);
            } else {
                if (S_ISDIR(path_stat.st_mode)) {
                    // It's a directory, try to remove it (will only work if empty)
                    if (rmdir(tokens[1]) != 0) {
                        if (errno == ENOTEMPTY) {
                            fprintf(output, "Error: Directory '%s' is not empty\n", tokens[1]);
                        } else {
                            fprintf(output, "Error: Could not remove directory '%s': %s\n",
                                    tokens[1], strerror(errno));
                        }
                    }
                } else {
                    // It's a regular file or link, try to unlink it
                    if (unlink(tokens[1]) != 0) {
                        fprintf(output, "Error: Could not remove '%s': %s\n",
                                tokens[1], strerror(errno));
                    }
                }
            }
        }
    }
    else if (strcmp(tokens[0], "mv") == 0) {
        if (token_count < 3) {
            fprintf(output, "Error: mv command requires source and destination arguments\n");
            fprintf(output, "Usage: mv source destination\n");
        } else {
            struct stat src_stat, dest_stat;
            char dest_path[PATH_MAX];
            int dest_exists = (stat(tokens[2], &dest_stat) == 0);
            
            // Check if source exists
            if (stat(tokens[1], &src_stat) == -1) {
                fprintf(output, "Error: Source '%s' does not exist\n", tokens[1]);
                return;
            }
            
            // Determine the full destination path
            if (dest_exists && S_ISDIR(dest_stat.st_mode)) {
                // If destination is a directory, append source filename
                const char *src_name = strrchr(tokens[1], '/');
                src_name = src_name ? src_name + 1 : tokens[1];
                snprintf(dest_path, sizeof(dest_path), "%s/%s", tokens[2], src_name);
            } else {
                strncpy(dest_path, tokens[2], sizeof(dest_path) - 1);
                dest_path[sizeof(dest_path) - 1] = '\0';
            }
            
            // Check if source and destination are the same
            if (strcmp(tokens[1], dest_path) == 0) {
                fprintf(output, "Error: Source and destination are the same file\n");
                return;
            }
            
            // Perform the move operation
            if (rename(tokens[1], dest_path) != 0) {
                // If rename fails, try to handle cross-device moves for regular files
                if (errno == EXDEV && S_ISREG(src_stat.st_mode)) {
                    // Cross-device move: need to copy and delete
                    FILE *src = fopen(tokens[1], "rb");
                    if (!src) {
                        fprintf(output, "Error: Could not open source file '%s': %s\n",
                                tokens[1], strerror(errno));
                        return;
                    }
                    
                    FILE *dest = fopen(dest_path, "wb");
                    if (!dest) {
                        fclose(src);
                        fprintf(output, "Error: Could not create destination file '%s': %s\n",
                                dest_path, strerror(errno));
                        return;
                    }
                    
                    // Copy the file contents
                    char buffer[8192];
                    size_t bytes;
                    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
                        if (fwrite(buffer, 1, bytes, dest) != bytes) {
                            fprintf(output, "Error: Failed to write to destination file '%s': %s\n",
                                    dest_path, strerror(errno));
                            fclose(src);
                            fclose(dest);
                            unlink(dest_path);  // Clean up partial file
                            return;
                        }
                    }
                    
                    // Preserve file permissions
                    fchmod(fileno(dest), src_stat.st_mode);
                    
                    // Close files
                    fclose(src);
                    fclose(dest);
                    
                    // Remove source file
                    if (unlink(tokens[1]) != 0) {
                        fprintf(output, "Warning: Moved file but could not remove source '%s': %s\n",
                                tokens[1], strerror(errno));
                    }
                } else {
                    fprintf(output, "Error: Could not move '%s' to '%s': %s\n",
                            tokens[1], dest_path, strerror(errno));
                }
            }
        }
    }
    else {
        // Try to execute the command as an external program
        pid_t pid = fork();
        
        if (pid == -1) {
            // Fork failed
            fprintf(output, "Error: Failed to create child process: %s\n", strerror(errno));
        } else if (pid == 0) {
            // Child process
            
            // If output was redirected to a file, redirect stdout
            if (output != stdout) {
                if (dup2(fileno(output), STDOUT_FILENO) == -1) {
                    fprintf(stderr, "Error: Failed to redirect output: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
                fclose(output);  // Close the file descriptor as it's no longer needed
            }
            
            // Execute the command
            execvp(tokens[0], tokens);
            
            // If execvp returns, it means it failed
            fprintf(stderr, "Error: Failed to execute '%s': %s\n", tokens[0], strerror(errno));
            exit(EXIT_FAILURE);
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
            
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                fprintf(output, "Program '%s' exited with status %d\n", 
                        tokens[0], WEXITSTATUS(status));
            }
        }
    }

    freeTokens(tokens, token_count);
    if (output != stdout) {
        fclose(output);
    }
}
