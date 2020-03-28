
#include "CheckSudokuWithMain.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

const char DIGITS[] = { '0', '1' };

int main(int argc, char **argv) {
	rowscolsmatrix RowsOrColsOrMatrix;
	int board[81];
	int readReturnValue = 0;
	char answer = '0';

	// if we don't have the type of process as argument, we have error
	// types can be: 0 - rows, 1 - columns, 2 - matrices
	if (argc == 2) {
		// runs until the pipe from where we read is closed
		for (;;) {
			RowsOrColsOrMatrix = (rowscolsmatrix) (*(argv[1]) - '0');
			readReturnValue = read(STDIN_FILENO, board, 81 * sizeof(int));
			// if pipe is closed, exit
			if (readReturnValue == 0) {
				break;
			}
			// true = '1', false = '0'
			answer = DIGITS[CheckRowsOrColsOrMatrix(RowsOrColsOrMatrix, board)];
			// writes answer to answer pipe
			write(STDOUT_FILENO, &answer, sizeof(char));
		}
	} else {
		fprintf(stderr,
				"ERROR in %s() function from \"%s\" file, line %d: %s\n",
				__FUNCTION__, __FILE__, __LINE__, strerror(errno));
		exit(EXIT_FAILURE);
	}

	return 0;
}

// checks the rows or columns or matrices in the board
bool CheckRowsOrColsOrMatrix(rowscolsmatrix RowsOrColsOrMatrix,
		int* boardAsArray) {
	int i, j;
	unsigned int answer = 0;
	int board[9][9];

	fromBoardArrayToBoardMatrix(boardAsArray, board);

	for (i = 0; i < 9; i++) {
		answer = 0;
		for (j = 0; j < 9; j++) {
			if (RowsOrColsOrMatrix == rows)
				answer = answer | (1 << board[i][j]);
			else if (RowsOrColsOrMatrix == cols)
				answer = answer | (1 << board[j][i]);
			else {
				// RowsOrColsOrMatrix == matrix
				answer = answer
						| (1
								<< board[(j / 3) + (i / 3) * 3][(j % 3)
										+ (i % 3) * 3]);
			}
		}
		// every bit from bit 2, is an answer for a part of the board
		// we have 9 parts
		// answer of 1 means this part is correct
		// we need all parts to be correct in order to return: answer = true
		if (answer != 0b1111111110)
			return false;
	}
	return true;
}

// convert board from int[81] to int[9][9]
void fromBoardArrayToBoardMatrix(int* boardAsArray, int boardAsMatrix[9][9]) {
	int i, j;
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			boardAsMatrix[i][j] = *(boardAsArray + i * 9 + j);
		}
	}
}

