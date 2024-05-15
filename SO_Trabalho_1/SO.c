#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct _infos {
    double tempo;
    int qtdPrimos;
} Infos;

#define LINHA 10000
#define COLUNA 10000

srand();
int **matriz = criarMatriz(LINHA, COLUNA);

int **criarMatriz(LINHA, COLUNA) {
    matriz = (int**) calloc(LINHA, sizeof(int*));
    for (int i = 0; i < LINHA; i++) {
        matriz[i] = (int*) calloc(COLUNA, sizeof(int));
    }

    for (int i = 0; i < LINHA; i++) {
        for(int j = 0; j < COLUNA; j++) {
            matriz[i][j] = rand() % 32000;
        }
    }

    return matriz;
}


int ehPrimo(int n) {
    int raizQuadrada = sqrt(n);
    int contador = 2; 

    while (contador < raizQuadrada) {
        if (n % contador == 0) return 1;
        contador++;
    }

    return 0;
}

Infos BuscaSerial(float tempo) {
    
}



int main(int argc, char *argv[]) {
    
}