
#ifndef CHECKSUDOKU_H_
#define CHECKSUDOKU_H_

#include "utils.h"

bool CheckRowsOrColsOrMatrix(rowscolsmatrix RowsOrColsOrMatrix,
		int* boardAsArray);
void fromBoardArrayToBoardMatrix(int* boardAsArray, int boardAsMatrix[9][9]);

#endif /* CHECKSUDOKU_H_ */
