#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct _tempo {
  time_t tempo_inicial;
  time_t tempo_final;
} Tempo;

#define LINHA 10000
#define COLUNA 10000

/* variáveis globais */
int qtd_primos = 0; // contabiliza a quantidade de primos 
int **matriz; // matriz de inteiros

int **criarMatriz(int linha, int coluna) {
  matriz = (int **)calloc(LINHA, sizeof(int *));
  if (matriz == NULL) {
    printf("Erro ao alocar memória para a matriz.");
    EXIT_FAILURE;
  }

  for (int i = 0; i < LINHA; i++) {
    matriz[i] = (int *)calloc(COLUNA, sizeof(int));
    if (matriz[i] == NULL) {
      printf("Erro ao alocar memória para a coluna.");
      EXIT_FAILURE;
    }
  }

  for (int i = 0; i < LINHA; i++) {
    for (int j = 0; j < COLUNA; j++) {
      matriz[i][j] = rand() % 32000;
    }
  }

  return matriz;
}

int ehPrimo(int n) {
  int raizQuadrada = sqrt(n);
  int contador = 2;

  while (contador < raizQuadrada) {
    if (n % contador == 0)
      return 0;
    contador++;
  }

  return 1;
}

int main(int argc, char *argv[]) {
  /* semente pré definida */
  srand(3);

  matriz = criarMatriz(LINHA, COLUNA);

  /* serial */

  Tempo *tempo_serial = (Tempo *)calloc(1, sizeof(Tempo));
  time(&tempo_serial->tempo_inicial);

  for (int i = 0; i < LINHA; i++) {
    for (int j = 0; j < COLUNA; j++) {
      if (ehPrimo(matriz[i][j])) qtd_primos++;
    }
  }

  time(&tempo_serial->tempo_final);
  double tempo_execucao_serial = difftime(tempo_serial->tempo_final, tempo_serial->tempo_inicial);

  printf("Tempo: %f\nNumeros primos: %d", tempo_execucao_serial, qtd_primos);

  return 0;
}
