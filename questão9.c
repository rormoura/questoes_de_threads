#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>// para usar o bool data type

int TAM = 0;// tamanho do array
bool *escolhidos;//vetor de escolhidos durante o crivo
bool *vetor;//esse eh o vetor principal, o que vai conter, no final, os que sao primos e os que nao sao

pthread_mutex_t *mutexes_vetor;//mutexes referentes ao vetor principal
pthread_mutex_t *mutexes_escolhidos;//mutexes referentes ao vetor de escolhidos durante o crivo


void *verif_primos()
{
    int i = 0;//variavel iterativa
    int escolhido = 1;//o primo escolhido pela thread
    while(true)//as threads so terminam quando nao houver mais primos a serem encontrados
    {
        for(i = escolhido+1; i < TAM; i++)
        {//nesse for, a thread pega o seu primo respectivo
            pthread_mutex_lock(&mutexes_vetor[i]);//a thread so dorme aqui se:ou tem uma thread pegando esse primo antes dela
            //ou alguma thread ta eliminando vetor[i] no laco for que elimina os multiplos
            if(vetor[i] == true)//se i eh primo
            {
                pthread_mutex_lock(&mutexes_escolhidos[i]);// a thread so dorme aqui se tem uma thread pegando esse primo antes dela
                if(escolhidos[i] == false)//se i eh primo e ainda nao foi escolhido
                {
                    escolhidos[i] = true;//agora ele foi escolhido
                    escolhido = i;
                    pthread_mutex_unlock(&mutexes_escolhidos[i]);
                    pthread_mutex_unlock(&mutexes_vetor[i]);
                    break;
                }
                pthread_mutex_unlock(&mutexes_escolhidos[i]);
            }
            pthread_mutex_unlock(&mutexes_vetor[i]);
        }
        if(i == TAM)//se nao ha mais primos
        {
            break;//entao a thread termina
        }
        for(i = 2; i*escolhido < TAM; i++)
        {//nesse for, eliminamos os multiplos
            pthread_mutex_lock(&mutexes_vetor[i*escolhido]);
            vetor[i*escolhido] = false;
            pthread_mutex_unlock(&mutexes_vetor[i*escolhido]);
        }
    }
}

int main()
{
    int N = 0;
    int T = 0;
    int i = 0;// variavel iterativa
    printf("Digite o numero N: ");
    scanf("%d", &N);
    printf("Digite o numero T: ");
    scanf("%d", &T);
    TAM = N+1;

    pthread_t vetor_threads[T];//vetor de threads
    vetor = (bool *) malloc(TAM*sizeof(bool));//alocando memoria para o vetor principal
    escolhidos = (bool *) malloc(TAM*sizeof(bool));//alocando memoria para o vetor de escolhidos durante o crivo
    mutexes_escolhidos = (pthread_mutex_t *) malloc(TAM*sizeof(pthread_mutex_t));//alocando memoria para os vetores de mutexes
    mutexes_vetor = (pthread_mutex_t *) malloc(TAM*sizeof(pthread_mutex_t));
    if(vetor == NULL || escolhidos == NULL || mutexes_vetor == NULL || mutexes_escolhidos == NULL)
    {
        printf("Erro de alocacao\n");
        exit(-1);
    }
    for(i = 0; i < TAM; i++)
    {
        vetor[i] = true;//inicializando o vetor de booleanos
        escolhidos[i] = false;//inicializando o vetor de escolhidos
        pthread_mutex_init(&mutexes_escolhidos[i], NULL);//inicializando os vetores dos mutexes
        pthread_mutex_init(&mutexes_vetor[i], NULL);
    }


    vetor[0] = false;//nem zero nem um são primos
    vetor[1] = false;

    
    for(i = 0; i < T; i++)
    {//criando as threads
        pthread_create(&vetor_threads[i], NULL, verif_primos, NULL);
    }


    for(i = 0; i < T; i++)
    {//aguardando as threads terminarem
        pthread_join(vetor_threads[i], NULL);
    }


    for(i = 2; i < TAM; i++)
    {//printando os primos de 2 ate N
        if(vetor[i] == true)
        {
            printf("\t%d\n", i);
        }
    }


    //parte da limpeza
    for(i = 0; i < TAM; i++)
    {
        pthread_mutex_destroy(&mutexes_vetor[i]);
        pthread_mutex_destroy(&mutexes_escolhidos[i]);
    }
    free(vetor);
    free(escolhidos);
    free(mutexes_escolhidos);
    free(mutexes_vetor);


    pthread_exit(NULL);
    return 0;
}