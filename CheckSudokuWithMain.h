
#ifndef CHECKSUDOKUWITHMAIN_H_
#define CHECKSUDOKUWITHMAIN_H_

#include "utils.h"

bool CheckRowsOrColsOrMatrix(rowscolsmatrix RowsOrColsOrMatrix,
		int* boardAsArray);
void fromBoardArrayToBoardMatrix(int* boardAsArray, int boardAsMatrix[9][9]);

#endif /* CHECKSUDOKUWITHMAIN_H_ */
