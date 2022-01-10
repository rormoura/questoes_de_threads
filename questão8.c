#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex_quant_pass = PTHREAD_MUTEX_INITIALIZER;
//mutex de acesso à quantidade de passageiros na fila
int quant_pass = 0;//quantidade de passageiros na fila
int fila[20] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};//a fila de passageiros, inicialmente vazia (todos preenchidos com -1)

pthread_t passageiros[20];//threads que representam os passageiros
int enjoados[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//vetor que indica se um passageiro está enjoado ou não
//inicialmente todos não estão enjoados (0), mas logo após uma thread andar na montanha russa, ela fica enjoada (1)

typedef struct 
{//struct que representa os argumentos de uma thread
    int id;//identificação de um passageiro
}argumentos;

pthread_cond_t varcond_fila = PTHREAD_COND_INITIALIZER;//variável de condição em relação referente a um passageiro estar ou não na fila

void * func_carrinho()//função que representa o carrinho
{
    int i = 0;//variável iterativa
    int voltas = 1;//contador de voltas
    while(voltas < 11)//o carrinho só realiza dez voltas
    {
        pthread_mutex_lock(&mutex_quant_pass);
        if(quant_pass == 10)//se o carrinho está cheio, pode sair
        {
            printf("VOLTA %d: ", voltas);
            quant_pass = 0;//retira os passageiros da fila e os transporta para o carrinho
            for(i = 0; i < 20; i++)
            {
                if(fila[i] != -1)//se um passageiro i estava ocupando a fila
                {
                    printf("%d ", i);//a cada volta são printados os passageiros correspondentes
                    enjoados[i] = 1;//agora ele está enjoado
                    fila[i] = -1;//e ele não está mais na fila
                }
            }
            printf("\n");
            voltas++;
            pthread_cond_broadcast(&varcond_fila);//depois de uma volta, acorda as threads que estavam no carrinho (dormindo)
        }
        pthread_mutex_unlock(&mutex_quant_pass);
    }
}

void * func_pass(void *args)//função que representa um passageiro
{
    int indice = ((argumentos *)args)->id;
    while(1)//os passageiros tentam entrar no carrinho até que são finalizadas
    {
        pthread_mutex_lock(&mutex_quant_pass);
        if(quant_pass < 10 && enjoados[indice] == 0)//então ainda há vagas na fila e não estou enjoado
        {
            fila[indice] = indice;//ocupando a vaga na fila
            quant_pass++;
            pthread_cond_wait(&varcond_fila, &mutex_quant_pass);//depois de entrar na fila, é posta para dormir
        }
        else
        {//se não há mais vagas na fila
            enjoados[indice] = 0;//isso equivale à passeada no parque antes de retornar à fila
        }
        pthread_mutex_unlock(&mutex_quant_pass);
    }
}

int main()
{
    int i = 0;//variável iterativa

    pthread_t carrinho; // thread que representa o carrinho
    pthread_create(&carrinho, NULL, func_carrinho, NULL);

    argumentos args[20];//argumentos dos 20 passageiros
    for(i = 0; i < 20; i++)
    {
        args[i].id = i;
        pthread_create(&passageiros[i], NULL, func_pass, (void *) &args[i]);
    }

    pthread_join(carrinho, NULL);//esperando a thread carrinho terminar
//não esperamos pelo término das threads passageiros pq o programa acaba com as dez voltas do carrinho
    return 0;
}