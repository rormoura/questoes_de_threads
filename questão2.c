#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
int TAM = 0;//tamanho do vetor
int intervalo = 0; // intervalo = TAM/N
int ordenado = 1; // o vetor, a priori, esta ordenado
//se, durante a execucao, alguma thread encontrar que o array n esta
//ordenado, entao a variavel "ordenado" vira zero.
pthread_mutex_t mutex_ordenado = PTHREAD_MUTEX_INITIALIZER;// mutex para a variavel "ordenado"
typedef struct // struct que representa os argumentos das threads 
{
    int id;// identificador da thread
    int *v; // o vetor
}argumentos;

void verif_ord(void * args)// funcao que representa cada thread
{
    int num = ((argumentos *)args)->id;// identificador da thread
    int inicio = intervalo*num; // inicio da particao da thread
    int fim = inicio + intervalo; // fim da particao da thread + 1
    int *vetor = ((argumentos *)args)->v; // o vetor
    int i = 0; // variavel iterativa
    if(inicio < (fim-1))// verifica se a particao tem mais de dois elementos
    {// aqui dentro verificamos se o trecho esta ordenado
        for(i = inicio; i < (fim-1); i++)
        {
            if(vetor[i] > vetor[i+1])
            {
                pthread_mutex_lock(&mutex_ordenado);
                ordenado = 0; // o vetor nao esta ordenado
                pthread_mutex_unlock(&mutex_ordenado);
                return;
            }
        }
    }
    if(inicio > 0)// verifica se o "inicio" tem algum elemento antes dele
    {// aqui dentro verificamos se o inicio esta ordenado com seu elemento anterior
        if(vetor[inicio-1] > vetor[inicio])
        {
            pthread_mutex_lock(&mutex_ordenado);
            ordenado = 0; // o vetor nao esta ordenado
            pthread_mutex_unlock(&mutex_ordenado);
            return;
        }
    }
    if(fim < (TAM-1))//verifica se o "fim" tem algum elemento depois dele
    {//aqui dentro verificamos se o fim esta ordendo com seu elemento posterior
        if(vetor[fim-1] > vetor[fim])
        {
            pthread_mutex_lock(&mutex_ordenado);
            ordenado = 0; // o vetor nao esta ordenado
            pthread_mutex_unlock(&mutex_ordenado);
            return;
        }
    }
}

int main()
{
    int *vetor = NULL; // vetor que vai ser preenchido
    int i = 0; // variavel iterativa
    int N = 0; // numero de threads
    printf("Digite o tamanho do vetor: ");
    scanf("%d", &TAM);
    printf("Digite os %d elementos do vetor\n", TAM);
    for(i = 0; i < TAM; i++)// recebendo os valores do vetor
    {
        vetor = (int *) realloc(vetor, (i+1)*sizeof(int));
        printf("Posicao %d: ", i);
        scanf("%d", &vetor[i]);
    }
    printf("Printando o vetor\n");
    for(i = 0; i < TAM; i++)// printando o vetor
    {
        printf("%d%c", vetor[i], (i == TAM-1) ? '\n' : ' ');
    }

    printf("Digite o numero N: ");// recebendo N
    scanf("%d", &N);

    intervalo = TAM/N;// particao de cada thread
    argumentos vetor_args[N];// vetor de argumentos das threads
    pthread_t vetor_threads[N];// vetor de threads

    for(i = 0; i < N; i++)// criando as threads
    {
        vetor_args[i].id = i;
        vetor_args[i].v = vetor;
        pthread_create(&vetor_threads[i], NULL, (void *) verif_ord, (void *) &vetor_args[i]);
    }

    for(i = 0; i < N; i++)// aguardando as threads terminarem
    {
        pthread_join(vetor_threads[i], NULL);
    }

    if(ordenado == 1)
    {
        printf("O vetor esta ordenado\n");
    }
    else
    {
        printf("O vetor nao esta ordenado\n");
    }

    free(vetor);
    pthread_exit(NULL);
    return 0;
}