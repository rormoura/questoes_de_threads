#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct 
{
    int r,g,b;//valores correspondentes ao red, green e blue de cada pixel
}pixel;

typedef struct 
{
    int linha, coluna;//linha e coluna da respectiva thread
    pixel **matriz_pixeis;
}argumentos;//struct que representa os argumentos recebidos pela funcao conversao

int **matriz_convertida;//matriz que vai armazenar os valores dos pixeis em escala de cinza
pixel **matrix;//matrix que vai armazenar os valores dos pixeis em rgb

void * conversao(void *args)
{
    int i = ((argumentos *)args)->linha;//linha do pixel respectivo da thread
    int j = ((argumentos *)args)->coluna;//coluna do pixel respectivo da thread
    int red = ((argumentos *)args)->matriz_pixeis[i][j].r;//valor red do pixel respectivo da thread
    int green = ((argumentos *)args)->matriz_pixeis[i][j].g;//valor green do pixel respectivo da thread
    int blue = ((argumentos *)args)->matriz_pixeis[i][j].b;//valor blue do pixel respectivo da thread
    matriz_convertida[i][j] = (int) (red*0.3 + green*0.59 + blue*0.11);//formula de conversao
}

int main()
{
    int i = 0, j = 0;// variaveis iterativas
    int linhas = 0, colunas = 0;//quantidade de linhas e colunas da ppm
    FILE *fp;//ponteiro para o arquivo
    fp = fopen("entrada2.bin", "rb");
    fseek(fp, 3, 1);//pula o "P3\n"
    fscanf(fp, "%d %d", &linhas, &colunas);
    matriz_convertida = (int **) malloc(linhas*sizeof(int *));
    matrix = (pixel **) malloc(linhas*sizeof(pixel *));
    for(i = 0; i < linhas; i++)
    {
        for(j = 0; j < colunas; j++)
        {
            matriz_convertida[i] = (int *) malloc(colunas*sizeof(int));
            matrix[i] = (pixel *) malloc(colunas*sizeof(pixel));
        }
    }
    fseek(fp, 4, 1);//pula o \n e valor maximo possivel para cada cor e o \n
    for(i = 0; i < linhas; i++)
    {
        for(j = 0; j < colunas; j++)
        {
            if(i <linhas-1 && i < colunas-1)
            {
                fscanf(fp, "%d %d %d", &matrix[i][j].r, &matrix[i][j].g, &matrix[i][j].b);
                fseek(fp, 1, 1);//pula o \n entre as linhas
            }
            else
            {
                fscanf(fp, "%d %d %d", &matrix[i][j].r, &matrix[i][j].g, &matrix[i][j].b);
            }
        }
    }
    fclose(fp);//fechando o arquivo
    for(i = 0; i < linhas; i++)
    {
        for(j = 0; j < colunas; j++)
        {//aqui printamos a matriz em rgb
            printf("%d %d %d", matrix[i][j].r, matrix[i][j].g, matrix[i][j].b);
            printf("\n");
        }
    }

    // a partir daqui usamos threads
    pthread_t matriz_threads[linhas][colunas];//matriz de threads
    argumentos matriz_args[linhas][colunas];//matriz dos argumentos das threads
    //obs.: criamos uma matriz de threads para isolar a conversao de um pixel
    for(i = 0; i < linhas; i++)
    {
        for(j = 0; j < colunas; j++)
        {
            matriz_args[i][j].linha = i;
            matriz_args[i][j].coluna = j;
            matriz_args[i][j].matriz_pixeis = matrix;
            pthread_create(&matriz_threads[i][j], NULL, conversao, (void *) &matriz_args[i][j]);
        }
    }
    for(i = 0; i < linhas; i++)
    {
        for(j = 0; j < colunas; j++)
        {//aguardando as threads terminarem
            pthread_join(matriz_threads[i][j], NULL);
        }
    }

    for(i = 0; i < linhas; i++)
    {
        for(j = 0; j < colunas; j++)
        {//printando a matriz convertida
            printf("%d%c", matriz_convertida[i][j], (j == colunas-1) ? '\n' : ' ');
        }
    }

    fp = fopen("saida2.bin", "wb");//escrevendo no arquivo final a matriz em escala de cinza
    for(i = 0; i < linhas; i++)
    {
        for(j = 0; j < colunas; j++)
        {
            fprintf(fp, "%d%c", matriz_convertida[i][j], (j == colunas-1) ? '\n' : ' ');
        }
    }
    fclose(fp);

    pthread_exit(NULL);
    return 0;
}