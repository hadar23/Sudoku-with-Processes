
#ifndef PROCESSESOPTION2_H_
#define PROCESSESOPTION2_H_

#include "utils.h"

typedef struct {
	int board[81];
	bool boardReady;
	bool result[3];
	bool resultReady[3];
	bool noMoreBoards;
} sudoku_t;

int creatThreeProcesses(pid_t *pid, rowscolsmatrix *RowsOrColsOrMatrix);
int parentAct(int argc, char **argv, sudoku_t *sudokuMem);
void childAct(sudoku_t *sudokuMem, rowscolsmatrix RowsOrColsOrMatrix);

#endif /* PROCESSESOPTION2_H_ */
