all: v1 v2 CSWM

v1:
	gcc -o v1 ProcessesOption1.c utils.c
v2:
	gcc -o v2 ProcessesOption2.c CheckSudoku.c utils.c
CSWM:
	gcc -o CSWM CheckSudokuWithMain.c utils.c
clean:
	rm -f v1 v2 CSWM
