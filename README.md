# Sudoku-with-Processes
Sudoku with Processes in C, second year second semester's first Operating Systems project

### Sudoku Puzzle
This type of puzzle has a 9-row and 9-column matrix when some entries have digits (between 1 and 9) and some of the entries are empty.  Digits must be entered in the empty entries so that the following conditions are met:
1) Each of the rows will display all digits between 1 and 9
2) Each of the columns will display all digits between 1 and 9
3) Each of the nine *3x3* "sub-matrices" will display all digits between 1 and 9

### Checker Interface
The program reads from an input file (one or more), a digits matrix (one matrix per file) and checks for each matrix if it is a valid Sudoku puzzle solution. If the matrix is a valid solution, then the program prints the message "FILENAME is legal" to standard output. If the matrix is not a valid solution, he program prints the message "FILENAME is not legal" (where FILENAME is the file name in which the matrix was saved).  
The input file names will be passed to the program as a command line argument (in argv). If there is no command line argument then the program reads a single matrix from the standard input.  
Input file contains 81 digits separated by white space (spaces, newlines or tabs). Input appears as the first row digits (from left to right), followed by the second row digits and so on to ninth row digits.

### Sudoku Checker with Processes
The main process creates three child processes by calling *fork*. After creating the three children, the main process reads the input files (one by one) and pass the content (the matrix) to the child processes.  
Checking the validity of each matrix will be shared among the child processes.  
One of the three child processes is checking if all the lines are valid (line is valid if it contains all digits between 1 and 9).  
The second is checking the validity all columns, and the third child process is checking the validity of each *3x3* "sub-matrices".  
The three children reports back to the main process the results of the tests they have done.  
The main process will print to standard output the appropriate message.

### Communication Between Processes
The main process needs to communicate with his children: The children need to know what is the proposed solution for the
Sudoku (81 digits) so they can check their validity. The main process reads the solution from the input and sends it to the child Processes.  
Each child should report back to the parent (the main process) the test result it did ("valid" or "not valid").  
There are many ways to do the communication between the processes. This exercise uses *pipes* and using shared memory by the *mmap* system call.

#### First Version: Using *execve* and *pipes*
Each child is using the *execve* system call to run a executable file with a program that does the test by the child (checking the rows, columns or sub-matrices validity).  
The three child processes run the same executable file while using command line arguments to specify the type of test to perform (test the rows, columns, or sub-matrices).

The main process communicates with the children using *pipes*. For each child, the main (parent) process creates a pipe.  
The parent writes the matrix (the solution proposed for the Sudoku puzzle) to the pipe and the child reads from the pipe.
For the test results, there is one more pipe into which each of the children writes the result of the test it did.  
Each child writes a single byte to the pipe (whose value is 1 if "valid" and 0 if "not valid") and the main process reads the three bytes out of it.

#### Second Version: Using *mmap*
The main process communicates with the children using shared memory for it and its children.  
The main process uses the *mmap* system call to create the shared memory area.  
The call to *mmap* is done before the calls to *fork*, so that the child procceses inherit the mapping (Because calling to execve cancels the sharing created when calling *mmap*, we didn't use *execve* system call).

The shared memory area contains this data structure:
```
typedef struct {
	int board[81];
	bool boardReady;
	bool result[3];
	bool resultReady[3];
	bool noMoreBoards;
} sudoku_t;
```
1) board - the solution to the Sudoku we need to check
2) boardReady - a boolean that tells the child processes they can read the board from the memory
3) result - the answers of the child processes
4) resultReady - a boolean that tells the main process it can read the results from the memory
5) noMoreBoards - a boolean that tells the child processes that there are no more boards
