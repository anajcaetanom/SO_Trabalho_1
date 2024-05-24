#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#pragma comment(lib,"pthreadVC2.lib")
#define HAVE_STRUCT_TIMESPEC

#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define QTD_THREADS 6

#define LINHA 10000
#define COLUNA 10000

#define MACRO_LINHA 100
#define MACRO_COLUNA 100

#define MATRIZ_OCUPACAO_DIMENSAO (LINHA / MACRO_LINHA)

#define TOTAL_MACROBLOCOS ((LINHA * COLUNA) / (MACRO_LINHA * MACRO_COLUNA))

/* variáveis globais */
pthread_mutex_t mutex, mutex2; // variáveis mutex.
int qtd_primos = 0; // quantidade de primos.
int** matriz; // matriz de inteiros aleatórios.
int matrizOcupacao[MATRIZ_OCUPACAO_DIMENSAO][MATRIZ_OCUPACAO_DIMENSAO]; // matriz que armazena a ocupação de cada macrobloco.


/*
* Aloca e preenche a matriz.
* @param linha: quantidade de linhas da matriz.
* @param coluna: quantidade de colunas da matriz.
* @return matriz bidimensional de inteiros.
*/
int** criarMatriz(int linha, int coluna) {

    matriz = (int**)calloc(LINHA, sizeof(int*));

    if (matriz == NULL) {
        printf("Erro ao alocar memória para a matriz.");
        EXIT_FAILURE;
    }

    for (int i = 0; i < LINHA; i++) {
        matriz[i] = (int*)calloc(COLUNA, sizeof(int));
        if (matriz[i] == NULL) {
            printf("Erro ao alocar memória para as colunas.");
            EXIT_FAILURE;
        }
    }

    // preenchimento da matriz
    for (int i = 0; i < LINHA; i++) {
        for (int j = 0; j < COLUNA; j++) {
            matriz[i][j] = rand() % 32000;
        }
    }

    /*
    // teste
    int count = 1;
    for (int i = 0; i < LINHA; i++) {
        for (int j = 0; j < COLUNA; j++) {
            matriz[i][j] = count;
            //printf("%d   ", matriz[i][j]);
            count++;
        }
        //printf("\n");
    }
    */

    return matriz;
}

int ehPrimo(int n) {
    double raizQuadrada = sqrt(n);
    int contador = 2;

    if (n <= 1) return 0;

    while (contador <= raizQuadrada) {
        if (n % contador == 0) 
            return 0;
        
        contador++;
    }

    return 1;
}

void buscaSerial() {

    for (int i = 0; i < LINHA; i++) {
        for (int j = 0; j < COLUNA; j++) {
            if (ehPrimo(matriz[i][j])) 
                qtd_primos++;
        }
    }

}

/*
void printMatrizMacrobloco() {

    for (int bi = 0; bi < MACRO_LINHA; bi++) {
        for (int bj = 0; bj < MACRO_COLUNA; bj++) {
            printf("Bloco (%d, %d):\n", bi, bj);
            for (int k = 0; k < MACRO_LINHA; k++) {
                for (int l = 0; l < MACRO_COLUNA; l++) {
                    int x = bi * MACRO_LINHA + k;
                    int y = bj * MACRO_COLUNA + l;
                    if (x < LINHA && y < COLUNA) {
                        printf("%d ", matriz[x][y]);
                    }
                    else {
                        printf("-1 ");
                    }
                }
                printf("\n");
            }
            printf("\n");
        }
    }

}
*/

void* buscaParalela(void* arg) {
    int row_start, row_end, col_start, col_end;
    
    for (int i = 0; i < MATRIZ_OCUPACAO_DIMENSAO; i++) {
        for (int j = 0; j < MATRIZ_OCUPACAO_DIMENSAO; j++) {

            pthread_mutex_lock(&mutex);

            if (matrizOcupacao[i][j] == 0) {
                matrizOcupacao[i][j] = 1;

                pthread_mutex_unlock(&mutex);

                row_start = i * MACRO_LINHA;
                row_end = (i + 1) * MACRO_LINHA;
                col_start = j * MACRO_COLUNA;
                col_end = (j + 1) * MACRO_COLUNA;

                for (int k = row_start; k < row_end; k++) {
                    for (int l = col_start; l < col_end; l++) {
                        
                        if (ehPrimo(matriz[k][l])) {
                            pthread_mutex_lock(&mutex2);
                            qtd_primos++;
                            pthread_mutex_unlock(&mutex2);
                        }
                        
                    }
                }
            } 
            
            else {
                pthread_mutex_unlock(&mutex);
            }

            
        }
    }

    pthread_exit(NULL);
}




int main(int argc, char* argv[]) {

    srand(3); // semente pré-definida.
    matriz = criarMatriz(LINHA, COLUNA); // alocação e preenchimento da matriz.

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex2, NULL);

    clock_t tempo_inicial_serial, tempo_final_serial;
    clock_t tempo_inicial_paralelo, tempo_final_paralelo;

    pthread_t array_threads[QTD_THREADS]; // array de threads.

    /**************** SERIAL ****************/   

    tempo_inicial_serial = clock();
    buscaSerial();
    tempo_final_serial = clock();
    
    double tempo_execucao_serial = (double)(tempo_final_serial - tempo_inicial_serial) / CLOCKS_PER_SEC;
    
    printf("Tempo serial: %f\nNumeros primos: %d\n", tempo_execucao_serial, qtd_primos);
    
    
    /**************** PARALELO ****************/   

    qtd_primos = 0;    

    // matriz dos macroblocos com todas as ocupações livres
    for (int i = 0; i < MACRO_LINHA; i++) {
        for (int j = 0; j < MACRO_COLUNA; j++) 
            matrizOcupacao[i][j] = 0;
    }       

    tempo_inicial_paralelo = clock();

    for (int thread = 0; thread < QTD_THREADS; thread++) {
        pthread_create(&array_threads[thread], NULL, buscaParalela, NULL);  
    }

    for (int thread = 0; thread < QTD_THREADS; thread++) {
        pthread_join(array_threads[thread], NULL);
    }

    tempo_final_paralelo = clock();

    double tempo_execucao_multithread =  (double)(tempo_final_paralelo - tempo_inicial_paralelo) / CLOCKS_PER_SEC;

    printf("Tempo multithread: %f\nNumeros primos: %d\n", tempo_execucao_multithread, qtd_primos);

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex2);


    

    return 0;
}