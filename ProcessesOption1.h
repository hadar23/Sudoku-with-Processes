
#ifndef PROCESSESOPTION1_H_
#define PROCESSESOPTION1_H_

void creatThreeProcesses(int **pFdR, int **pFdW1, int **pFdW2, int **pFdW3,
		char* argv0);
void parentAct(int argc, char **argv, int *fdR, int *fdW1, int *fdW2, int *fdW3);

#endif /* PROCESSESOPTION1_H_ */
