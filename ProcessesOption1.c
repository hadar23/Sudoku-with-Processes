
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "utils.h"
#include "ProcessesOption1.h"

const char* DIGITS[] = { "0", "1", "2" };
// for eclipse
//const char* SUDOKU_CHECK_FILE_PATH = "../CheckSudoku/Debug/CheckSudoku";
// for terminal
// const char* SUDOKU_CHECK_FILE_PATH = "../../CheckSudoku/Debug/CheckSudoku";
// for makefile
const char* SUDOKU_CHECK_FILE_PATH = "./CSWM";

int main(int argc, char **argv) {
	int fdR[2], fdW1[2], fdW2[2], fdW3[2];
	int *pFdR = fdR, *pFdW1 = fdW1, *pFdW2 = fdW2, *pFdW3 = fdW3;
	int a;

	// checks if the path to the "CheckSudoku" file is correct
	a = access(SUDOKU_CHECK_FILE_PATH, F_OK);
	if (a < 0) {
		fprintf(stderr,
				"ERROR in %s() function from \"%s\" file, line %d: %s\n",
				__FUNCTION__, __FILE__, __LINE__, strerror(errno));
		exit(EXIT_FAILURE);
	}

	// creates three child processes and send them to work in another program
	creatThreeProcesses(&pFdR, &pFdW1, &pFdW2, &pFdW3, argv[0]);
	// the parent action
	parentAct(argc, argv, fdR, fdW1, fdW2, fdW3);

	return 0;
}

// parent action, get the boards and send to children processes for checking. print answer
void parentAct(int argc, char **argv, int *fdR, int *fdW1, int *fdW2, int *fdW3) {
	bool readFromInput = false, goodAnswer = true;
	int numberOfBoards = 0, i, j;
	int board[81];
	char boardAsString[STRING_BOARD_SIZE];
	int fd = 0;
	char answer[3];
	char* nameOfFile = "matrix";

	// if we don't have board files as arguments, we will read from the input
	if (argc == 1) {
		readFromInput = true;
		numberOfBoards = 1;
	} else
		// if we have board files as arguments
		numberOfBoards = argc - 1;

	// run loop for all the boards
	for (i = 1; i <= numberOfBoards; i++) {
		// read board from input
		if (readFromInput == true) {
			readBoardFromInput(boardAsString, 0);
		} else { // read board from file
			nameOfFile = argv[i];
			fd = open(nameOfFile, O_RDONLY);
			if (fd == -1) {
				fprintf(stderr,
						"ERROR in %s() function from \"%s\" file, line %d: %s\n",
						__FUNCTION__, __FILE__, __LINE__, strerror(errno));
				exit(EXIT_FAILURE);
			}

			// the input is now the file
			readBoardFromInput(boardAsString, fd);
			close(fd);
		}

		// convert the board from string to int[81]
		convertStringBoardToIntBoard(boardAsString, STRING_BOARD_SIZE, board);

		// send the board to children via pipes
		write(fdW1[1], board, 81 * sizeof(int));
		write(fdW2[1], board, 81 * sizeof(int));
		write(fdW3[1], board, 81 * sizeof(int));

		// read answer from children via pipes
		read(fdR[0], answer + 0, sizeof(char));
		read(fdR[0], answer + 1, sizeof(char));
		read(fdR[0], answer + 2, sizeof(char));

		// checks the answers of the children
		goodAnswer = true;
		for (j = 0; j < 3; j++) {
			if (answer[j] == '0') {
				goodAnswer = false;
				break;
			}
		}

		// prints if the board is legal or not
		if (goodAnswer)
			fprintf(stdout, "%s is legal\n", nameOfFile);
		else
			fprintf(stdout, "%s is not legal\n", nameOfFile);

	}

	// close pipes (so children processes can exit)
	close(fdW1[1]);
	close(fdW2[1]);
	close(fdW3[1]);
	close(fdR[0]);

	// another way of inducing exit in children processes
	// signal(1, SIG_IGN);
	// kill(-getpid(), 1);

}

// creates three child processes that will communicate with parent via pipes
// all child will use the same "read" pipe
// every child will have a special "write" pipe with parent
// "write" pipe means: parent writes to it and child reads
// "read" pipe means: parent reads from it what child writes
void creatThreeProcesses(int **pFdR, int **pFdW1, int **pFdW2, int **pFdW3,
		char* argv0) {
	int i, execlpReturn = 0;
	pid_t pid;
	int ***pWriteFd = NULL;
	rowscolsmatrix RowsOrColsOrMatrix;

	// create the "read" pipe that all children will have
	if (pipe(*pFdR)) {
		fprintf(stderr,
				"ERROR in %s() function from \"%s\" file, line %d: %s\n",
				__FUNCTION__, __FILE__, __LINE__, strerror(errno));
		exit(EXIT_FAILURE);
	}

	// create the "write" pipe for every child process
	for (i = 0; i < 3; i++) {
		switch (i) {
		case 0:
			pWriteFd = &pFdW1;
			break;
		case 1:
			pWriteFd = &pFdW2;
			break;
		case 2:
			pWriteFd = &pFdW3;
			break;
		}

		// create the "write" pipe that the child and parent will use
		if (pipe(**pWriteFd)) {
			fprintf(stderr,
					"ERROR in %s() function from \"%s\" file, line %d: %s\n",
					__FUNCTION__, __FILE__, __LINE__, strerror(errno));
			exit(EXIT_FAILURE);
		}

		// create the child process
		pid = fork();

		// if we have an error
		if (pid < 0) {
			fprintf(stderr,
					"ERROR in %s() function from \"%s\" file, line %d: %s\n",
					__FUNCTION__, __FILE__, __LINE__, strerror(errno));
			exit(EXIT_FAILURE);
		} else if (pid == 0) { // if we are in child
			close((**pWriteFd)[1]);	// close writing edge in children "write" pipe
			close((*pFdR)[0]); // close reading edge in children "read" pipe
			// connect stdin to reading edge in children "write" pipe
			if ((**pWriteFd)[0] != STDIN_FILENO) {
				dup2((**pWriteFd)[0], STDIN_FILENO);
				close((**pWriteFd)[0]);
			}
			// connect stdout to writing edge in children "read" pipe
			if ((*pFdR)[1] != STDOUT_FILENO) {
				dup2((*pFdR)[1], STDOUT_FILENO);
				close((*pFdR)[1]);
			}
			RowsOrColsOrMatrix = (rowscolsmatrix) i;
			// go to another program and wait for boards to check
			execlpReturn = execlp(SUDOKU_CHECK_FILE_PATH, argv0,
					DIGITS[RowsOrColsOrMatrix], (char*) NULL);
			// if we have an error
			if (execlpReturn < 0) {
				fprintf(stderr, "%s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
		// close reading edge in parent "write" pipe
		close((**pWriteFd)[0]);
	}
	// close writing edge in parent "read" pipe
	close((*pFdR)[1]);
}

