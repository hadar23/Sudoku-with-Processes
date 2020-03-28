
#ifndef UTILS_H_
#define UTILS_H_

#define STRING_BOARD_SIZE 300

typedef enum {
	rows, cols, matrix
} rowscolsmatrix;
typedef enum {
	false, true
} bool;

void readBoardFromInput(char* boardAsString, int fd);
void convertStringBoardToIntBoard(char* boardAsString, int stringSize,
		int* board);
void printSudokuBoard(int* board);

#endif /* UTILS_H_ */
