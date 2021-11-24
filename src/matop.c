#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <string.h>
#include "mat.h"
#include "memlog.h"
#include "msgassert.h"

// definicoes de operacoes a serem testadas
#define OPSOMAR 1
#define OPMULTIPLICAR 2
#define OPTRANSPOR 3

// variaveis globais para opcoes
static int opescolhida;
int leituraArquivos;
int matriz2;

//nome do arquivo em que serao feitos os registros de acesso
char lognome[100];
int optx, opty, regmem, regmen2;

//nomes dos arquivos de entrada e saida de matrizes
char arq1[100];
char arq2[100];
char arq3[100];

// Descricao: imprime as opcoes de uso
// Entrada: nao tem
// Saida: impressao das opcoes de linha de comando
void uso(){

  fprintf(stderr, "matop\n");
  fprintf(stderr, "\t-s \t(somar matrizes) \n");
  fprintf(stderr, "\t-m \t(multiplicar matrizes) \n");
  fprintf(stderr, "\t-t \t(transpor matriz)\n");
  fprintf(stderr, "\t-p <arq>\t(arquivo de registro de acesso)\n");
  fprintf(stderr, "\t-l \t(registrar acessos a memoria)\n");
  fprintf(stderr, "\t-x <int>\t(primeira dimensao)\n");
  fprintf(stderr, "\t-y <int>\t(segunda dimensao)\n");
}

// Descricao: le as opcoes da linha de comando e inicializa variaveis
// Entrada: argc e argv
// Saida: optescolhida, optx, opty, regmem, lognome
void parse_args(int argc, char **argv){

  // variaveis externas do getopt
  extern char *optarg;
  extern int optind;

  // variavel auxiliar
  int c;

  // inicializacao variaveis globais para opcoes
  leituraArquivos = 0;
  matriz2 = 0;
  opescolhida = -1;
  optx = -1;
  opty = -1;
  regmem = 0;
  regmen2 = 0;
  lognome[0] = 0;
  arq1[0] = 0;
  arq2[0] = 0;
  arq3[0] = 0;

  // getopt - letra indica a opcao, : junto a letra indica parametro
  // no caso de escolher mais de uma operacao, vale a ultima
  while ((c = getopt(argc, argv, "smtp:x:y:lh1:2:o:")) != EOF)
    //printf("\nOLHA O C: %c\n", c);
    switch (c){
      case 'm':
        avisoAssert(opescolhida == -1, "Mais de uma operacao escolhida");
        opescolhida = OPMULTIPLICAR;
        break;
      case 's':
        avisoAssert(opescolhida == -1, "Mais de uma operacao escolhida");
        opescolhida = OPSOMAR;
        break;
      case 't':
        avisoAssert(opescolhida == -1, "Mais de uma operacao escolhida");
        opescolhida = OPTRANSPOR;
        break;
      case 'p':
        regmem = 1;
        strcpy(lognome, optarg);
        break;
      case 'x':
        optx = atoi(optarg);
        break;
      case 'y':
        opty = atoi(optarg);
        break;
      case 'l':
        regmen2 = 1;
        break;
      case '1':
        leituraArquivos = 1;
        strcpy(arq1, optarg);
        break;
      case '2':
        matriz2 = 1;
        strcpy(arq2, optarg);
        break;
      case 'o':
        strcpy(arq3, optarg);
        break;
      default:
        uso();
        exit(1);
    }

  // verificacao da consistencia das opcoes, quando aplicaveis
  erroAssert(!(leituraArquivos == 1 && optx > 0 && opty > 0),
             "Leitura de arquivos e geracao de matrizes aleatorias acionados simultaneamente");
  
  erroAssert(opescolhida > 0, "matop - necessario escolher operacao");
  
  if (regmen2){
    erroAssert(strlen(lognome) > 0,
               "matop - nome de arquivo de registro de acesso tem que ser definido");
  }

  if (!leituraArquivos){
    erroAssert(optx > 0, "matop - dimensao X da matriz tem que ser positiva");
    erroAssert(opty > 0, "matop - dimensao Y da matriz tem que ser positiva");
  }

  erroAssert(!(opescolhida==OPTRANSPOR && matriz2==1), "A tranposicao de matrizes apenas recebe uma matriz de entrada");
}

// Descricao: programa principal para execucao de operacoes de matrizes
// Entrada: argc e argv
// Saida: depende da operacao escolhida
int main(int argc, char **argv){

  // ate 3 matrizes sao utilizadas, dependendo da operacao
  mat_tipo a, b, c;

  // avaliar linha de comando
  parse_args(argc, argv);

  // iniciar registro de acesso, se solicitado
  if (regmem){
    iniciaMemLog(lognome);
  }

  // ativar registro de acesso ou nao, se solicitado
  if (regmen2){
    ativaMemLog();
  }
  else{
    desativaMemLog();
  }

  // execucao dependente da operacao escolhida
  switch (opescolhida){
    
    //cria matrizes a e b aleatorias ou as le de um arquivo
    //faz a soma de a e b e armazena na matriz c
    //matriz c é impressa e escrita em um arquivo, se solicitado
    //todas as matrizes sao desalocadas
    case OPSOMAR:

      //fase 0
      defineFaseMemLog(0);
      if (leituraArquivos){
        leMatrizArquivo(&a, arq1, 0);
        leMatrizArquivo(&b, arq2, 1);
      }
      else{
        criaMatriz(&a, optx, opty, 0);
        inicializaMatrizAleatoria(&a);
        criaMatriz(&b, optx, opty, 1);
        inicializaMatrizAleatoria(&b);
      }
      criaMatriz(&c, a.tamx, a.tamy, 2);
      inicializaMatrizNula(&c);

      //fase 1
      defineFaseMemLog(1);
      acessaMatriz(&a);
      acessaMatriz(&b);
      acessaMatriz(&c);
      somaMatrizes(&a, &b, &c);

      //fase 2
      defineFaseMemLog(2);
      acessaMatriz(&c);
      if (leituraArquivos){
        imprimeMatrizArquivo(&c, arq3);
      }
      imprimeMatriz(&c);
      destroiMatriz(&a);
      destroiMatriz(&b);
      destroiMatriz(&c);
      break;

    //cria matrizes a e b aleatorias ou as le de um arquivo
    //faz a multiplicacao de a e b e armazena na matriz c
    //matriz c é impressa e escrita em um arquivo, se solicitado
    //todas as matrizes sao desalocadas
    case OPMULTIPLICAR:

      //fase 0
      defineFaseMemLog(0);
      if (leituraArquivos){
        leMatrizArquivo(&a, arq1, 0);
        leMatrizArquivo(&b, arq2, 1);
      }
      else{
        criaMatriz(&a, optx, opty, 0);
        inicializaMatrizAleatoria(&a);
        criaMatriz(&b, optx, opty, 1);
        inicializaMatrizAleatoria(&b);
      }
      criaMatriz(&c, a.tamx, a.tamx, 2);
      inicializaMatrizNula(&c);

      //fase 1
      defineFaseMemLog(1);
      acessaMatriz(&a);
      acessaMatriz(&b);
      acessaMatriz(&c);
      multiplicaMatrizes(&a, &b, &c);

      //fase 2
      defineFaseMemLog(2);
      acessaMatriz(&c);
      if (leituraArquivos){
        imprimeMatrizArquivo(&c, arq3);
      }
      imprimeMatriz(&c);
      destroiMatriz(&a);
      destroiMatriz(&b);
      destroiMatriz(&c);
      break;

    //cria matrizes a aleatoria ou a le de um arquivo
    //faz a transposicao na propria matriz
    //matriz é impressa e escrita em um arquivo, se solicitado
    //as matriz e desalocadas
    case OPTRANSPOR:
      
      //fase 0
      defineFaseMemLog(0);
      if (leituraArquivos){
        leMatrizArquivo(&a, arq1, 0);
      }
      else{
        criaMatriz(&a, optx, opty, 0);
        inicializaMatrizAleatoria(&a);
      }

      //fase 1
      defineFaseMemLog(1);
      acessaMatriz(&a);
      transpoeMatriz(&a);

      //fase 2
      defineFaseMemLog(2);
      acessaMatriz(&a);
      imprimeMatriz(&a);
      if (leituraArquivos){
        imprimeMatrizArquivo(&a, arq3);
      }
      destroiMatriz(&a);
      break;

    default:
      // nao deve ser executado, pois ha um erroAssert em parse_args
      uso();
      exit(1);
  }

  // conclui registro de acesso, se incializado
  if (regmem){
    return finalizaMemLog();
  }

  else{
    return 0;
  }
}