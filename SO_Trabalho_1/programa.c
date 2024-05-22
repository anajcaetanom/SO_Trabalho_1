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

#define LINHA 9
#define COLUNA 9

#define MACRO_LINHA 3
#define MACRO_COLUNA 3


typedef struct _tempo {
    double tempo_inicial;
    double tempo_final;
} Tempo;

typedef struct _macrobloco {
    int ocupacao;
} Macrobloco;


/* variáveis globais */
int qtd_primos = 0; // contabiliza a quantidade de primos 
int** matriz;// matriz de inteiros
Macrobloco* array_macroblocos; // 
int row_start, row_end;
int col_start, col_end;


int** criarMatriz(int linha, int coluna) {
    matriz = (int**)calloc(LINHA, sizeof(int*));
    if (matriz == NULL) {
        printf("Erro ao alocar memória para a matriz.");
        EXIT_FAILURE;
    }

    for (int i = 0; i < LINHA; i++) {
        matriz[i] = (int*)calloc(COLUNA, sizeof(int));
        if (matriz[i] == NULL) {
            printf("Erro ao alocar memória para a coluna.");
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
            printf("%d   ", matriz[i][j]);
            count++;
        }
        printf("\n");
    }

    return matriz;
}

int ehPrimo(int n) {
    double raizQuadrada = sqrt(n);
    int contador = 2;

    if (n <= 1) {
        return 0;
    }

    while (contador <= raizQuadrada) {
        if (n % contador == 0) {
            return 0;
        }
        else {
            contador++;
        }
    }

    return 1;
}

int cont_primos_serial(Tempo* tempo_serial) {
    tempo_serial->tempo_inicial = clock();

    for (int i = 0; i < LINHA; i++) {
        for (int j = 0; j < COLUNA; j++) {
            if (ehPrimo(matriz[i][j])) qtd_primos++;
        }
    }

    tempo_serial->tempo_final = clock();

    return qtd_primos;
}


void PrintmatrizMacrobloco(){
    int num_blocos_linhas = LINHA / MACRO_LINHA ;
    int num_blocos_colunas = COLUNA  / MACRO_COLUNA;

    for (int bi = 0; bi < num_blocos_linhas; bi++) {
        for (int bj = 0; bj < num_blocos_colunas; bj++) {
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
    Macrobloco* macrobloco = (Macrobloco*)arg;
    int qtd_primos_temp = 0; // variável temporária que contabiliza a quantidade de números primos dentro do macrobloco.
    pthread_mutex_t mutex; // declaração da variável mutex.
    pthread_mutex_init(&mutex, NULL); // cria e inicializa o mutex lock.

    // verifica se macrobloco tá desocupado.
    if (macrobloco->ocupacao == 0) {
        macrobloco->ocupacao = 1; // marca o macrobloco como ocupado.

        // percorre o macrobloco.
        for (int linha = row_start; linha < row_end; linha++) {
            for (int coluna = col_start; coluna < col_end; coluna++) {
                if (ehPrimo(matriz[linha][coluna])) // verifica se o elemento matriz[linha][coluna] é primo.
                    qtd_primos_temp++;             
            }
        }

        // região crítica.
        pthread_mutex_lock(&mutex); // adquire o mutex lock.

        qtd_primos += qtd_primos_temp; // incrementa a quantidade de nºs primos dentro do macrobloco na variável global.

        pthread_mutex_unlock(&mutex); // libera mutex lock.
        pthread_mutex_destroy(&mutex); // desaloca as estruturas do mutex.

    }
}

void qualthreadqualmacroblobo() {

}


int main(int argc, char* argv[]) {

    srand(3); // semente pré-definida.

    matriz = criarMatriz(LINHA, COLUNA); // alocação e preenchimento da matriz.


    // SERIAL

    Tempo* tempo_serial = (Tempo*)calloc(1, sizeof(Tempo));

    qtd_primos = cont_primos_serial(tempo_serial);

    double tempo_execucao_serial = (tempo_serial->tempo_final - tempo_serial->tempo_inicial) / CLOCKS_PER_SEC;

    printf("Tempo: %f\nNumeros primos: %d\n", tempo_execucao_serial, qtd_primos);

    PrintmatrizMacrobloco();


    
    
    // MULTITHREAD

    int total_macroblocos = (LINHA * COLUNA) / (MACRO_LINHA * MACRO_COLUNA);
     
    array_macroblocos = (Macrobloco*)calloc(total_macroblocos, sizeof(Macrobloco));

    if (array_macroblocos == NULL) {
        printf("Erro ao alocar o array de macroblocos.");
        EXIT_FAILURE;
    }

    // inicialização dos macroblocos desocupados
    for (int j = 0; j < total_macroblocos; j++) {
        array_macroblocos[j].ocupacao = 0;
    }

    pthread_t array_threads[QTD_THREADS_FISICAS]; // array de threads
    
    // cálculo das coordenadas do macrobloco.
    for (int i = 0; i < MACRO_LINHA; i++) {
        for (int j = 0; j < MACRO_COLUNA; j++) {

            row_start = i * MACRO_LINHA;
            row_end = (i + 1) * MACRO_LINHA;
            col_start = j * MACRO_COLUNA;
            col_end = (j + 1) * MACRO_COLUNA;

            // inicialização das threads nos macroblocos    
            for (int macrobloco_id = 0; macrobloco_id < total_macroblocos; macrobloco_id++) {



                for (int thread = 0; thread < QTD_THREADS_FISICAS; thread++) {
                    pthread_create(&array_threads[thread], NULL, buscaParalela, &array_macroblocos[macrobloco_id]);
                }
            }
        }
    }

    return 0;
}