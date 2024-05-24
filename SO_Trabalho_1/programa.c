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

#define QTD_THREADS_FISICAS 6
#define QTD_THREADS_LOGICAS 12

#define LINHA 1000
#define COLUNA 1000

#define MACRO_LINHA 10
#define MACRO_COLUNA 10

#define NUM_BLOCOS_LINHA  (LINHA / MACRO_LINHA)
#define NUM_BLOCOS_COLUNA  (COLUNA  / MACRO_COLUNA)

typedef struct _macrobloco {
    int ocupacao;
} Macrobloco;


/* variáveis globais */
pthread_mutex_t mutex, mutex2; // declaração da variável mutex.
int qtd_primos = 0; // contabiliza a quantidade de primos 
int** matriz; // matriz de inteiros
Macrobloco* array_macroblocos; // 
int row_start, row_end;
int col_start, col_end;

int macrobloco_id = 0;
int thread = 0;


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

    /*
    // preenchimento da matriz
    for (int i = 0; i < LINHA; i++) {
        for (int j = 0; j < COLUNA; j++) {
            matriz[i][j] = rand() % 32000;
        }
    }
    */

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


void printMatrizMacrobloco() {

    for (int bi = 0; bi < NUM_BLOCOS_LINHA; bi++) {
        for (int bj = 0; bj < NUM_BLOCOS_COLUNA; bj++) {
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


void* buscaParalela(void* arg) {
    int macrobloco_id = (int)arg;

    // int qtd_primos_temp = 0; // variável temporária que contabiliza a quantidade de números primos dentro do macrobloco.

    // verifica se macrobloco tá desocupado.
    if (array_macroblocos[macrobloco_id].ocupacao == 0) {
        pthread_mutex_lock(&mutex2);
        array_macroblocos[macrobloco_id].ocupacao = 1; // marca o macrobloco como ocupado.
        pthread_mutex_unlock(&mutex2);

        // percorre o macrobloco.
        for (int linha = row_start; linha < row_end; linha++) {
            for (int coluna = col_start; coluna < col_end; coluna++) {

                // verifica se o elemento matriz[linha][coluna] é primo.
                if (ehPrimo(matriz[linha][coluna])) { 
                    //printf("A thread %d esta no macrobloco %d numero da matriz %d.\n", thread, macrobloco_id, matriz[linha][coluna]);
                    
                    pthread_mutex_lock(&mutex);
                    qtd_primos++;
                    pthread_mutex_unlock(&mutex);
                    //printf("Primos: %d\n\n", qtd_primos);
                }
            }
        }
    }
    
    return NULL;
}




int main(int argc, char* argv[]) {

    srand(3); // semente pré-definida.
    matriz = criarMatriz(LINHA, COLUNA); // alocação e preenchimento da matriz.

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex2, NULL);


    /**************** SERIAL ****************/

    clock_t tempo_inicial_serial;
    clock_t tempo_final_serial;

    tempo_inicial_serial = clock();
    buscaSerial();
    tempo_final_serial = clock();
    
    double tempo_execucao_serial = (double)(tempo_final_serial - tempo_inicial_serial) / CLOCKS_PER_SEC;
    
    printf("Tempo serial: %f\nNumeros primos: %d\n", tempo_execucao_serial, qtd_primos);
    
    
    /**************** PARALELO ****************/

    clock_t tempo_inicial_paralelo;
    clock_t tempo_final_paralelo;

    qtd_primos = 0;   
    pthread_t array_threads[QTD_THREADS_FISICAS]; // array de threads.
    int total_macroblocos = (LINHA * COLUNA) / (MACRO_LINHA * MACRO_COLUNA);    

    array_macroblocos = (Macrobloco*)calloc(total_macroblocos, sizeof(Macrobloco));

    if (array_macroblocos == NULL) {
        printf("Erro ao alocar o array de macroblocos.");
        EXIT_FAILURE;
    }    

    // inicialização dos macroblocos desocupados
    for (int j = 0; j < total_macroblocos; j++) 
        array_macroblocos[j].ocupacao = 0; 

    tempo_inicial_paralelo = clock();

    // cálculo das coordenadas do macrobloco.
    for (int i = 0; i < NUM_BLOCOS_LINHA; i++) {
        for (int j = 0; j < NUM_BLOCOS_COLUNA; j++) {

            row_start = i * MACRO_LINHA;
            row_end = (i + 1) * MACRO_LINHA;
            col_start = j * MACRO_COLUNA;
            col_end = (j + 1) * MACRO_COLUNA; 
            

            // inicialização das threads
            for (thread = 0; thread < QTD_THREADS_FISICAS; thread++) {
                pthread_create(&array_threads[thread], NULL, buscaParalela, (void *)macrobloco_id);
            }

            macrobloco_id++;
            
        }
    }

    tempo_final_paralelo = clock();

    double tempo_execucao_multithread =  (double)(tempo_final_paralelo - tempo_inicial_paralelo) / CLOCKS_PER_SEC;


    for (int i = 0; i < QTD_THREADS_FISICAS; i++) {
        pthread_join(array_threads[i], NULL);        
    }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex2);

    printf("Tempo multithread: %f\nNumeros primos: %d\n", tempo_execucao_multithread, qtd_primos);

    

    return 0;
}