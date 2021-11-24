#ifndef MATH
#define MATH

#include "memlog.h"

#ifndef MAXTAM
#define MAXTAM 1000
#endif

typedef struct mat{
	double** m;
	int tamx, tamy;
	int id;
} mat_tipo;

double** alocaMatriz(int tx, int ty);
void desalocaMatriz(double** M, int tx, int ty);
void criaMatriz(mat_tipo* mat, int tx, int ty, int id);
void inicializaMatrizNula(mat_tipo* mat);
void inicializaMatrizAleatoria(mat_tipo * mat);
double acessaMatriz(mat_tipo * mat);
void imprimeMatriz(mat_tipo * mat);
void copiaMatriz(mat_tipo * src, mat_tipo * dst, int id);
void somaMatrizes(mat_tipo * a, mat_tipo * b, mat_tipo * c);
void multiplicaMatrizes(mat_tipo * a, mat_tipo * b, mat_tipo * c);
void transpoeMatriz(mat_tipo *a);
void destroiMatriz(mat_tipo *a);
void leMatrizArquivo(mat_tipo *a, char arq[], int id);
void imprimeMatrizArquivo(mat_tipo *a, char arq[]);

#endif