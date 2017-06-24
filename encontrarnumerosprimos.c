#include <stdio.h>
//#include <mpi.h>
#include <time.h>
#include <math.h>
// Programa Paralelo para cálculo de números primos
// Disciplina: Programação Paralela
// Prof. Francisco (Kiko)
// Aluno: Ederson Ribeiro Silva
// Data: 30/05/17

//Para o melhor desenvolvimento do paralelismo, utilizei a técnica de envio "Buffered" padrão do MPI_SEND



#define max_lin 10000000
#define proc_raiz 0

int primos[max_lin];
clock_t inicioTempo, fimTempo;

int main(int argc, char **argv){
    int primos[15];
    primos[0]=2;
    for(int i = 1; i<15; i++){
      primos[i]=0;
    }
    int index =0, size=1, max =15 , n=3;
    int teste;
    while(index < max){
      teste=1;
      if(n % 2 != 0){
        for(int i =0 ; i<size; i++){
            if(n % primos[i] == 0){
              teste = 0;
            }
        }
        if(teste){
          index++;
          size++;
          primos[index]=n;
        }
      }
      n++;
    }

    printf("Vetor de primos: \n");
    for (int i=0; i < 15; i++) {
      printf("%d\n", primos[i]);
    }
    return 0;
}
