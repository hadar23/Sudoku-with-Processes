
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "CheckSudoku.h"

const char DIGITS[] = { '0', '1' };

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
		if (answer != 0b1111111110)
			return false;
	}
	return true;
}

void fromBoardArrayToBoardMatrix(int* boardAsArray, int boardAsMatrix[9][9]) {
	int i, j;
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			boardAsMatrix[i][j] = *(boardAsArray + i * 9 + j);
		}
	}
}

