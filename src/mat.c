#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <getopt.h>
#include <string.h>
#include "mat.h"
#include "memlog.h"
#include "msgassert.h"

// limite superior da inicializacao aleatoria
#define INITRANDOMRANGE 10

// Descricao: aloca dinamicamente matriz com dimensoes tx X ty
// Entrada: tx, ty
// Saida: M
double** alocaMatriz(int tx, int ty){
    
    // verifica se os valores de tx e ty são validos
    erroAssert(tx>0,"Dimensao nula");
    erroAssert(ty>0,"Dimensao nula");
    erroAssert(tx<=MAXTAM,"Dimensao maior que permitido");
    erroAssert(ty<=MAXTAM,"Dimensao maior que permitido");

    int i = 0;
    //alocacao dinamica das tx linhas da matriz
    double** M = (double**)malloc(tx*sizeof(double*));

    //verifica se a alocacao dinamica de M ocorreu sem falhas
    erroAssert(M!=NULL,"Alocacao dinamica para M falhou");

    //alocacao dinamica das ty colunas da matriz
    for(i=0; i<tx; i++){
        M[i] = (double*)malloc(ty*sizeof(double));

        //verifica se a alocacao dinamica de M[i] ocorreu sem falhas
        erroAssert(M[i]!= NULL,"Alocacao dinamica para uma das linhas de M falhou");
    }

    return M;
}

// Descricao: desaloca memoria alocada dinamicamente para M
// Entrada: M, tx, ty
// Saida: N/A
void desalocaMatriz(double** M, int tx, int ty){
    int i;
    
    //desalocacao dos vetores em cada linha da matriz
    for(i=0; i<tx; i++){
        free(M[i]);
    }

    //desalocacao do vetor de vetores da matriz
    free(M);
}

// Descricao: cria matriz com dimensoes tx X ty e identificacao id
// Entrada: mat, tx, ty, id
// Saida: N/A
void criaMatriz(mat_tipo* mat, int tx, int ty, int id){
    
    // verifica se os valores de tx e ty são validos
    erroAssert(tx>0,"Dimensao nula");
    erroAssert(ty>0,"Dimensao nula");
    erroAssert(tx<=MAXTAM,"Dimensao maior que permitido");
    erroAssert(ty<=MAXTAM,"Dimensao maior que permitido");

    //aloca dinamicamente a matriz
    mat->m = alocaMatriz(tx, ty);

    // inicializa as dimensoes da matriz
    mat->tamx = tx;
    mat->tamy = ty;

    //inicializa o identificador da matriz, para rastreamento
    mat->id = id;
}

// Descricao: inicializa mat com valores nulos 
// Entrada: mat
// Saida: N/A
void inicializaMatrizNula(mat_tipo* mat){
    int i, j;
    
    // inicializa todos os elementos da matriz com 0
    for (i=0; i<mat->tamx; i++){
        for(j=0; j<mat->tamy; j++){
            mat->m[i][j] = 0;
            ESCREVEMEMLOG((long int)(&(mat->m[i][j])),sizeof(double), mat->id);
        }
    }
}

// Descricao: inicializa mat com valores aleatorios
// Entrada: mat 
// Saida: N/A
void inicializaMatrizAleatoria(mat_tipo * mat){
    
    //inicializa matriz como nula, por seguranca
    inicializaMatrizNula(mat);
    
    int i, j;    
    // inicializa matriz com valores aletorios entre 0 e INITRANDOMRANGE
    for (i=0; i<mat->tamx; i++){
        for(j=0; j<mat->tamy; j++){
            mat->m[i][j] = drand48()*INITRANDOMRANGE;
            ESCREVEMEMLOG((long int)(&(mat->m[i][j])),sizeof(double), mat->id);
        }
    }
}

// Descricao: acessa mat para fins de registro de acesso
// Entrada: mat
// Saida: N/A
double acessaMatriz(mat_tipo * mat){
    int i, j;
    double aux, s=0.0;
    for (i=0; i<mat->tamx; i++){
        for(j=0; j<mat->tamy; j++){
            aux = mat->m[i][j];
            s+=aux;
            LEMEMLOG((long int)(&(mat->m[i][j])),sizeof(double), mat->id);
        }
    }
    return s; // apenas para evitar que acesso seja eliminado
}


// Descricao: imprime a matriz com a identificacao de linhas e colunas
// Entrada: mat
// Saida: impressao na saida padrao (stdout) 
void imprimeMatriz(mat_tipo * mat){
    int i,j;

    // seguranca, mas erro não deve acontecer jamais
    erroAssert(mat->tamx<=MAXTAM,"Dimensao maior que permitido");
    erroAssert(mat->tamy<=MAXTAM,"Dimensao maior que permitido");

    // imprime os identificadores de coluna
    printf("%9s"," ");
    for(j=0; j<mat->tamy; j++){
        printf("%8d ",j);
    }
    printf("\n");

    // imprime as linhas
    for (i=0; i<mat->tamx; i++){
        printf("%8d ",i);
        for(j=0; j<mat->tamy; j++){
            printf("%8.2f ",mat->m[i][j]);
            LEMEMLOG((long int)(&(mat->m[i][j])),sizeof(double), mat->id);
        }
        printf("\n");
    }
}

// Descricao: faz uma copia de src em dst com identificacao id
// Entrada: src, dst, id
// Saida: N/A
void copiaMatriz(mat_tipo *src, mat_tipo * dst, int id){
    int i,j;

    // cria novamente a matriz dst para ajustar as suas dimensoes
    criaMatriz(dst, src->tamx, src->tamy, id);

    // inicializa a matriz dst como nula
    inicializaMatrizNula(dst);

    // copia os elementos da matriz src
    for (i=0; i<src->tamx; i++){
        for(j=0; j<src->tamy; j++){
            dst->m[i][j] = src->m[i][j];
            LEMEMLOG((long int)(&(src->m[i][j])),sizeof(double), src->id);
            ESCREVEMEMLOG((long int)(&(dst->m[i][j])),sizeof(double), dst->id);
        }
    }
}

// Descricao: soma as matrizes a e b e armazena o resultado em c
// Entrada: a, b, c
// Saida: N/A
void somaMatrizes(mat_tipo *a, mat_tipo *b, mat_tipo *c){
    int i,j;
    // verifica se as dimensoes das matrizes a e b sao as mesmas
    erroAssert(a->tamx==b->tamx,"Dimensoes incompativeis");
    erroAssert(a->tamy==b->tamy,"Dimensoes incompativeis");

    // inicializa a matriz c garantindo a compatibilidade das dimensoes
    criaMatriz(c, a->tamx, a->tamy, c->id);

    //seta todos os valores de c->m com zero, por seguranca
    inicializaMatrizNula(c);

    // faz a soma elemento a elemento
    for (i=0; i<a->tamx; i++){
        for(j=0; j<a->tamy; j++){
            c->m[i][j] = a->m[i][j]+b->m[i][j];
            LEMEMLOG((long int)(&(a->m[i][j])),sizeof(double), a->id);
            LEMEMLOG((long int)(&(b->m[i][j])),sizeof(double), b->id);
            ESCREVEMEMLOG((long int)(&(c->m[i][j])),sizeof(double), c->id);
        }
    }
}

// Descricao: multiplica as matrizes a e b e armazena o resultado em c
// Entrada: a, b, c
// Saida: N/A
void multiplicaMatrizes(mat_tipo *a, mat_tipo *b, mat_tipo *c){
    int i,j,k;
    // verifica a compatibilidade das dimensoes 
    erroAssert(a->tamy==b->tamx,"Dimensoes incompativeis");

    // cria e inicializa a matriz c
    criaMatriz(c,a->tamx, b->tamy, c->id);
    inicializaMatrizNula(c);

    // realiza a multiplicacao de matrizes
    for (i=0; i<c->tamx;i++){
        for (j=0; j<c->tamy;j++){
            for (k=0; k<a->tamy;k++){
                c->m[i][j] += a->m[i][k]*b->m[k][j];
                LEMEMLOG((long int)(&(a->m[i][k])),sizeof(double), a->id);
                LEMEMLOG((long int)(&(b->m[k][j])),sizeof(double), b->id);
                ESCREVEMEMLOG((long int)(&(c->m[i][j])),sizeof(double), c->id);
            }
        }
    }
}

// Descricao: transpoe a matriz a
// Entrada: a
// Saida: N/A
void transpoeMatriz(mat_tipo *a){

  //faz uma copia da antiga matriz, mas com um id diferente
  mat_tipo cpy;
  copiaMatriz(a, &cpy, 9);

  //desaloca a "antiga" matriz a
  desalocaMatriz(a->m, a->tamx, a->tamy);

  //aloca a "nova" matriz a com dimensoes transpostas 
  a->m = alocaMatriz(cpy.tamy, cpy.tamx);

  //seta os atributos correspondentes
  a->tamx = cpy.tamy;
  a->tamy = cpy.tamx;

  //inicializa nova matriz com todos os valores nulos, por seguranca
  inicializaMatrizNula(a);
  
  int i,j;
  // faz a transposicao como se a matriz fosse quadrada
  for (i=0; i<cpy.tamy; i++){
    for(j=0; j<cpy.tamx; j++){
      a->m[i][j]=cpy.m[j][i];
      LEMEMLOG((long int)(&(cpy.m[j][i])),sizeof(double), cpy.id);
      ESCREVEMEMLOG((long int)(&(a->m[i][j])),sizeof(double), a->id);
    }
  }
}

// Descricao: destroi a matriz a, que se torna inacessível, e desaloca memoria 
// alocada dinamicamente 
// Entrada: a
// Saida: N/A
void destroiMatriz(mat_tipo *a){
    // apenas um aviso se a matriz for destruida mais de uma vez
    avisoAssert(((a->tamx>0)&&(a->tamy>0)),"Matriz já foi destruida");

    //desaloca memoria alocada dinamicamente
    desalocaMatriz(a->m, a->tamx, a->tamy);

    // torna as dimensoes invalidas
    a->tamx = a->tamy = -1;
}

// Descricao: verifica correspondencia entre as dimensoes da matriz na
// linha do arquivo e seus dados propriamente ditos e 
// aborta o programa, em caso de inconsistencias
// Entrada: arq
// Saida: N/A
void confereMatrizArquivo(char arq[]){

    //variaveis auxiliares
    int x = 0;
    int y = 0;
    int i = 0;
    int espacos = 0;
    int numLinhas = 0;
    char buf[MAXTAM];
    FILE *arquivo;
    
    //abertura do arquivo e assercao correspondente
    arquivo = fopen(arq, "r");
    erroAssert(arquivo != NULL, "Nao foi possivel abrir o arquivo");

    //leitura das dimensoes da matriz
    fgets(buf, MAXTAM, arquivo);
    x = atoi(strtok(buf, " "));
    y = atoi(strtok(NULL, " "));

    //navega palas linhas e confere numero de elementos (numero de colunas)
    //ao final, confere numero de linhas
    while(!feof(arquivo)) {
        fgets(buf, MAXTAM, arquivo);
        numLinhas++;
        espacos = 0;
        i = 0;
        while(buf[i] != 0){
            if(buf[i] == 32) espacos++;
            i++;
        }
        erroAssert(espacos == y-1, "Matriz com dados faltantes ou sobressalentes");
    }
    erroAssert(numLinhas == x, "Matriz com dados faltantes ou sobressalentes");

    //fechamento do arquivo
    fclose(arquivo);
}

// Descricao: le uma matriz de uma arquivo arq e armazena suas informacoes em a
// cria e aloca a conforme informacoes lidas e identificacao id 
// Entrada: a, arq, id
// Saida: N/A
void leMatrizArquivo(mat_tipo *a, char arq[], int id){
    
    //confere se a matriz esta quebrada
    confereMatrizArquivo(arq);
    
    //variaveis auxiliares
    int i,j,x,y = 0;
    FILE *arquivo;

    //abertura do arquivo e assercao correspondente
    arquivo = fopen(arq, "r");
    erroAssert(arquivo != NULL, "Nao foi possivel abrir o arquivo");

    //leitura das dimensoes da matriz no arquivo
    fscanf(arquivo, "%d %d", &x, &y);
    erroAssert(x>0,"Dimensao nula");
    erroAssert(y>0,"Dimensao nula");
    erroAssert(x<=MAXTAM,"Dimensao maior que permitido");
    erroAssert(y<=MAXTAM,"Dimensao maior que permitido");

    //criacao e alocacao dinamica da matriz
    criaMatriz(a, x, y, id);
    
    //leitura da matriz propriamente dita
    for(i=0; i<a->tamx; i++){
        for(j=0; j<a->tamy; j++){
            fscanf(arquivo, "%lf", &a->m[i][j]);
            ESCREVEMEMLOG((long int)(&(a->m[i][j])), sizeof(double), a->id);
        }
    }

    //fechamento do arquivo
    fclose(arquivo);
}

// Descricao: imprime uma matriz a em um arquivo arq
// Entrada: a, arq
// Saida: N/A
void imprimeMatrizArquivo(mat_tipo *a, char arq[]){
//variaveis auxiliares
    int i,j;
    FILE *arquivo;

    //abertura do arquivo e assercao correspondente
    arquivo = fopen(arq, "w");
    erroAssert(arquivo != NULL, "Nao foi possivel abrir o arquivo");

    //escrita das dimensoes da matriz no arquivo
    fprintf(arquivo, "%d %d", a->tamx, a->tamy);
    fprintf(arquivo, "\n");

    //escrita da matriz propriamente dita
    for(i=0; i<a->tamx; i++){
        for(j=0; j<a->tamy; j++){
            fprintf(arquivo, "%lf ", a->m[i][j]);
            LEMEMLOG((long int)(&(a->m[i][j])), sizeof(double), a->id);
        }
        //para que uma linha vazia nao seja impressa ao final do arquivo
        if(i != a->tamx-1){
            fprintf(arquivo, "\n");
        }
    }

    //fechamento do arquivo
    fclose(arquivo);
}