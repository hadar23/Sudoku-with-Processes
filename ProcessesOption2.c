
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include "ProcessesOption2.h"
#include "utils.h"
#include "CheckSudoku.h"

int main(int argc, char **argv) {
	pid_t pid = 0;
	rowscolsmatrix RowsOrColsOrMatrix = 0;

	// create the mapping
	sudoku_t* sudokuMem = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
	MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	// if mapping failed, exit
	if (sudokuMem == MAP_FAILED) {
		fprintf(stderr,
				"ERROR in %s() function from \"%s\" file, line %d: %s\n",
				__FUNCTION__, __FILE__, __LINE__, strerror(errno));
		exit(EXIT_FAILURE);
	}

	// set all bits in mapping to zero (reset mapping)
	memset(sudokuMem, 0, sizeof(sudoku_t));

	// create three child processes
	// if we have an error signal the children proccesses that there are "noMoreBoards" so they can exit
	if (creatThreeProcesses(&pid, &RowsOrColsOrMatrix) == EXIT_FAILURE) {
		sudokuMem->noMoreBoards = true;
		exit(EXIT_FAILURE);
	}

	// if pid > 0, we are in parent process do parent action
	if (pid > 0) {
		if (parentAct(argc, argv, sudokuMem)) {
			sudokuMem->noMoreBoards = true;
			exit(EXIT_FAILURE);
		}
	} else
		// if we are in child process do child action
		childAct(sudokuMem, RowsOrColsOrMatrix);

	return 0;

}

// create three child processes
int creatThreeProcesses(pid_t *pid, rowscolsmatrix *RowsOrColsOrMatrix) {
	int i;
	for (i = 0; i < 3; i++) {
		*pid = fork();

		if (*pid < 0) {
			fprintf(stderr,
					"ERROR in %s() function from \"%s\" file, line %d: %s\n",
					__FUNCTION__, __FILE__, __LINE__, strerror(errno));
			return (EXIT_FAILURE);
		} else if (*pid == 0) {
			*RowsOrColsOrMatrix = (rowscolsmatrix) i;
			break;
		}
	}
	return 0;
}

// parent action, get the boards and send to children processes for checking. print answer
int parentAct(int argc, char **argv, sudoku_t *sudokuMem) {
	bool readFromInput = false, goodAnswer = true, answerReady = false;
	int numberOfBoards = 0, i, j;
	char boardAsString[STRING_BOARD_SIZE];
	int fd = 0;
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
		convertStringBoardToIntBoard(boardAsString, STRING_BOARD_SIZE,
				sudokuMem->board);
		// signal children the board is ready
		sudokuMem->boardReady = true;

		// stay in loop as long as the answers of the children are not ready
		while (!answerReady) {
			answerReady = true;
			for (j = 0; j < 3; j++) {
				if (sudokuMem->resultReady[j] == false) {
					answerReady = false;
					break;
				}
			}
		}

		// reset flags and read results
		answerReady = false;
		goodAnswer = true;
		sudokuMem->boardReady = false;
		for (j = 0; j < 3; j++) {
			sudokuMem->resultReady[j] = false;
			if (sudokuMem->result[j] == false) {
				goodAnswer = false;
			}
		}

		// print if the board is legal or not
		if (goodAnswer)
			fprintf(stdout, "%s is legal\n", nameOfFile);
		else
			fprintf(stdout, "%s is not legal\n", nameOfFile);
	}

	// signal children processes that there are "noMoreBoards" and they can exit
	sudokuMem->noMoreBoards = true;
	return 0;
}

// children action, get boards and check if they are legal. write answer in memory
void childAct(sudoku_t *sudokuMem, rowscolsmatrix RowsOrColsOrMatrix) {
	bool result = false;

	// stay in loop as long as there are more boards to come
	while (sudokuMem->noMoreBoards == false) {
		// if the board is ready and we didn't calculate the answer for it yet
		if (!(sudokuMem->resultReady[RowsOrColsOrMatrix])
				&& sudokuMem->boardReady) {
			result = CheckRowsOrColsOrMatrix(RowsOrColsOrMatrix,
					sudokuMem->board);
			sudokuMem->result[RowsOrColsOrMatrix] = result;
			sudokuMem->resultReady[RowsOrColsOrMatrix] = true;
		}

	}
	exit(0);
}

