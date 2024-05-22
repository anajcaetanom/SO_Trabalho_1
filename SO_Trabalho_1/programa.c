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

#define MACROBLOCO 3


typedef struct _tempo {
    double tempo_inicial;
    double tempo_final;
} Tempo;

typedef struct _macrobloco {
    int ocupacao;
} Macrobloco;


/* variáveis globais */
int qtd_primos = 0; // contabiliza a quantidade de primos 
int** matriz; // matriz de inteiros


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

int cont_primos_serial(Tempo *tempo_serial) {
    tempo_serial->tempo_inicial = clock();

    for (int i = 0; i < LINHA; i++) {
        for (int j = 0; j < COLUNA; j++) {
            if (ehPrimo(matriz[i][j])) qtd_primos++;
        }
    }

    tempo_serial->tempo_final = clock();

    return qtd_primos;
}

int cont_primos_multithread() {
    int aux = 0;
    // calcula o número total de macroblocos na matriz
    int num_blocos_linha = LINHA / MACROBLOCO;
    int num_blocos_coluna = COLUNA / MACROBLOCO;

    int total_macroblocos = num_blocos_linha * num_blocos_coluna;

    pthread_t array_threads[QTD_THREADS_FISICAS]; 

    Macrobloco* array_macroblocos = (Macrobloco*) calloc(total_macroblocos, sizeof(Macrobloco));

    if (array_macroblocos == NULL) {
        printf("Erro ao alocar o array de macroblocos.");
        EXIT_FAILURE;
    }

    // inicialização dos macroblocos desocupados
    for (int j = 0; j < total_macroblocos; j++)
        array_macroblocos[j].ocupacao == 0;

    // percorrendo a qtd de threads 
    for (int threads = 0; threads < QTD_THREADS_FISICAS; threads++) {
        // percorrendo a qtd de macroblocos
        for (int id_macrobloco = 0; id_macrobloco < total_macroblocos; id_macrobloco++) {
            // verifica se macrobloco ta desocupado
            if (array_macroblocos[id_macrobloco].ocupacao == 0) {
                // percorre a matriz e marca o macrobloco como ocupado
                array_macroblocos[id_macrobloco].ocupacao = 1;
                
                for (int linha = 0; linha < num_blocos_linha; linha++) {
                    for (int coluna = 0; coluna < num_blocos_coluna; coluna++) {
                        // verifica se o elemento matriz[linha][coluna] é primo
                        pthread_create(&array_threads[aux], NULL, ehPrimo, matriz[linha][coluna]);
                        aux++;

                    }
                }
            }

            else {

            }

        }
    }       
        
    // Loop através dos macroblocos
    for (int i = 0; i < num_blocos_linha; i++) {
        for (int j = 0; j < num_blocos_coluna; j++) {
            // Cria uma estrutura de dados para armazenar as informações do macrobloco
            Macrobloco* mb = (Macrobloco)malloc(sizeof(Macrobloco));
            mb->ocupacao = 0; // Inicialmente, o macrobloco está vazio

            // Determina os limites do macrobloco
            int row_start = i * MACROBLOCO;
            int row_end = (i + 1) * MACROBLOCO;
            int col_start = j * MACROBLOCO;
            int col_end = (j + 1) * MACROBLOCO;
            
            mb->ocupacao++
            
            pthread_create(&array_threads[aux], NULL, aux_primo_multithread(), row_start, row_end, col_start, col_end);
        }
    }
}

void aux_primo_multithread(int row_start, int row_end, int col_start, int col_end){
    // Loop através das células dentro do macrobloco
            for (int row = row_start; row < row_end; row++) {
                for (int col = col_start; col < col_end; col++) {
                    // Verifica se a célula contém um número primo
                    if (ehPrimo(matriz[row][col])) {
                        qtd_primos++;
                    }
                }
            }
}




int main(int argc, char* argv[]) {
    /* semente pré definida */
    srand(3);

}