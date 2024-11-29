# Custom Shell Implementation in C

A robust and feature-rich shell implementation written in C, demonstrating core concepts of operating systems, process management, and file operations.

## 🌟 Features

### Built-in Commands
- `pwd` - Print working directory
- `cd [dir]` - Change directory (supports relative/absolute paths and `cd` with no args)
- `ls [-l]` - List directory contents (with optional long format)
- `echo [text]` - Display text
- `clear` - Clear the screen
- `exit` - Exit the shell

### File Operations
- `cat [file]` - Display file contents
- `touch [file]` - Create new file or update timestamps
- `mkdir [dir]` - Create new directory
- `rm [file/dir]` - Remove file or empty directory
- `mv [source] [dest]` - Move/rename files and directories

### Advanced Features
- Output redirection using `>`
- External program execution
- Comprehensive error handling
- Cross-device file operations support
- Process management with proper child process handling

## 🛠️ Technical Implementation

### Core Components
- **Command Parsing**: Robust tokenization with support for spaces and special characters
- **Process Management**: Fork/exec model for external program execution
- **File Operations**: Low-level system calls for file manipulation
- **Error Handling**: Detailed error messages with errno support

### Key Files
- `edsh.c` - Main shell loop and initialization
- `commands.c` - Command implementations and execution logic
- `commands.h` - Header definitions and function declarations

## 🚀 Getting Started

### Prerequisites
- GCC compiler
- Unix-like operating system (Linux/macOS)
- Make build system

### Building the Shell
```bash
# Clone the repository
git clone [your-repo-url]
cd write-shell-in-C

# Build the shell
make

# Run the shell
./edsh
```

### Usage Examples
```bash
# Basic file operations
touch newfile.txt
echo "Hello World" > newfile.txt
cat newfile.txt
mv newfile.txt renamed.txt
rm renamed.txt

# Directory operations
mkdir testdir
cd testdir
pwd
ls -l
cd ..
rm testdir

# Running external programs
ps aux
date
uname -a
```

## 🔍 Implementation Details

### Command Execution Flow
1. Read input from user
2. Parse and tokenize command
3. Check for built-in commands
4. If not built-in, fork and execute as external program
5. Handle output redirection if present
6. Wait for command completion
7. Display results/errors

### Error Handling
- File existence checks
- Permission validations
- Process creation error handling
- System call failure management
- Detailed error messages with errno descriptions

### Memory Management
- Proper allocation/deallocation of command tokens
- File descriptor management
- Process cleanup

## 🤝 Contributing

Feel free to fork this project and submit pull requests. Some areas for potential enhancement:
- Pipeline implementation (`|` operator)
- Background process support (`&`)
- Command history
- Tab completion
- Signal handling
- Environment variable support

## 📝 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🙏 Acknowledgments

This project was created as a learning exercise in systems programming and operating system concepts. It demonstrates practical implementation of:
- Process management
- File system operations
- System calls
- Command line parsing
- Error handling

## 📞 Contact

Soutrik Maiti - [LinkedIn](https://www.linkedin.com/in/maitisoutrik/)

Project Link: [GitHub Repository](https://github.com/maitiSoutrik/write-shell-in-C)
