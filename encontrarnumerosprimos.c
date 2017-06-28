#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <math.h>
// Programa Paralelo para cálculo de números primos
// Disciplina: Programação Paralela
// Prof. Francisco (Kiko)
// Aluno: Ederson Ribeiro Silva
// Data: 30/05/17
//Para o melhor desenvolvimento do paralelismo, utilizei a técnica de envio "Buffered" padrão do MPI_SEND
#define max_lin 10000000
#define envio_prox_num 2001
#define ret_num_primo 2002
#define envio_vetor 3001
#define envio_tamanho 4001
#define LOOPING 5001
#define envio_vetor_enviados 6001
#define envio_cont 7001
#define rec_prime_buffer 8001
#define proc_raiz 0

int primos[max_lin];
clock_t inicioTempo, fimTempo;

int main(int argc, char **argv){
    int num_primo;
    int index;
    int tam =50;
    int continua = 1;
    MPI_Status status; //Definindo a variável "status"
    int ierr, my_id, num_procs; //Definições para o paralelismo
    for(int i = 1; i<max_lin; i++){
      primos[i]=0;
    }
    ierr = MPI_Init(&argc, &argv);
    /* Encontrando o ID do processo e quantos processos foram criados */
    /* find out MY process ID, and how many processes were started. */
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    //Garantindo que todo o vetor tenha valor 0 em suas posições

    if(my_id==proc_raiz){
      printf("Serao analisados os números primos entre 1 e o valor digitado pelo usuario\n");
      printf("Qual o ultimo numero do intervalo a ser verificado? ");
            scanf("%i", &tam); /* Último número do intervalo de primos */

      if ((tam <= 1)||(tam > max_lin)){ /* Número Inválido */
        printf("Valor digitado pelo usuario e invalido!");
        return(1);
      }
      inicioTempo= clock();
      int sizebuffer = num_procs-1;//tamanho do vetor de buffer
      int vetBuffer[sizebuffer];
      int indexBuffer=0;
      int cont = 0;// conadora
      int regcont=tam;// contadora regresiva
      int processo;
      int aux=2;

      while(aux<=tam){
        if(cont<1){
            int numerosenviados[sizebuffer];
            for(int i=0; i<sizebuffer; i++){
              numerosenviados[i]=aux++;
            }
            processo=1;
            ierr = MPI_Send(&continua, 1 , MPI_INT, processo, LOOPING, MPI_COMM_WORLD);//1*
            ierr = MPI_Send(&cont, 1 , MPI_INT, processo, envio_cont, MPI_COMM_WORLD);//2*
            ierr = MPI_Send(&numerosenviados[0], sizebuffer, MPI_INT, 1, envio_vetor_enviados, MPI_COMM_WORLD);//3*
            // envia para processos o vetor de enviados;

            for(int i=0; i<sizebuffer;i++){
              ierr = MPI_Recv( &num_primo, 1, MPI_INT, MPI_ANY_SOURCE, ret_num_primo, MPI_COMM_WORLD, &status);
              if(num_primo != 0){
                vetBuffer[indexBuffer]=num_primo;
                primos[index]=num_primo;
                index++;
                indexBuffer++;
              }
            }
            cont++;
        }else{
          for(processo=1;processo<num_procs;processo++){
            ierr = MPI_Send(&continua, 1 , MPI_INT, processo, LOOPING, MPI_COMM_WORLD);
            ierr = MPI_Send(&cont, 1 , MPI_INT, processo, envio_cont, MPI_COMM_WORLD);
            ierr = MPI_Send(&index, 1 , MPI_INT, processo, envio_tamanho, MPI_COMM_WORLD);
            ierr = MPI_Send(&aux, 1 , MPI_INT, processo, envio_prox_num, MPI_COMM_WORLD);
            if(indexBuffer==sizebuffer-1){
              ierr = MPI_Send(&vetBuffer[0], sizebuffer, MPI_INT, processo, envio_vetor, MPI_COMM_WORLD);
              indexBuffer=0;
            }
            aux++;

            // recebendo os numeros primos
            ierr = MPI_Recv( &num_primo, 1, MPI_INT, MPI_ANY_SOURCE, ret_num_primo, MPI_COMM_WORLD, &status);
            if(num_primo != 0){
              vetBuffer[indexBuffer]=num_primo;
              primos[index]=num_primo;
              index++;
              indexBuffer++;
            }
            cont++;
          }
        }
        continua =0;
      for(int i =1; i<num_procs; i++){
        ierr = MPI_Send(&continua, 1 , MPI_INT, processo, LOOPING, MPI_COMM_WORLD);
        }
      }
      int pos;
      for (pos=0; pos<=index; pos++)
        printf("%d ", primos[pos]);
 	     printf("\n");
 	     printf("Quantidade de primos encontrados: %d", index);

 	     /* Exibindo o tempo gasto na execução da aplicação */
 	     fimTempo= clock();
 	     printf("\n");
 	     printf("---------------------------------------------------------------------");
 	     printf("\n");
 	     printf("%f\n", (float)inicioTempo);
 	     printf("%f\n", (float)fimTempo);
 	     printf("Diferença em ms: %f\n",(float)((fimTempo-inicioTempo)/ 1000000.0F ) * 1000);

        ierr = MPI_Finalize();

 	   return(0); /* Tudo OK. Fim do Programa! */

    }else{
      int num_teste, teste;
      int index_primos;
      int ult_primo=0;
      int ult_enviado=0;
      int sizebuffer = num_procs-1;
      int vetBuffer[sizebuffer];
      int vetNumEnviados[sizebuffer];
      int cont =0;
      int continua = 1;

      for(int i=0; i<sizebuffer; i++){
        vetBuffer[i]=0;
        vetNumEnviados[i]=0;
      }

      ierr = MPI_Recv( &continua, 1, MPI_INT, proc_raiz, LOOPING, MPI_COMM_WORLD, &status);
      ierr = MPI_Recv( &cont, 1, MPI_INT, proc_raiz, envio_cont, MPI_COMM_WORLD, &status);
      while(continua){
      if(cont<1){
        index_primos=0;
        ierr = MPI_Recv( &vetNumEnviados, sizebuffer, MPI_INT, proc_raiz, envio_vetor_enviados, MPI_COMM_WORLD, &status);
        for(int i=0; i<sizebuffer; i++){
          if(vetNumEnviados[i]==2){
            vetBuffer[ult_primo]=vetNumEnviados[i];
            ult_enviado++;
          }else{
            for(int j=0; j<ult_enviado; j++){
              teste=1;
              if(vetNumEnviados[i] %2 == 0){
                teste=0;
              }else{
                if(vetNumEnviados[i] % vetBuffer[j] == 0){
                  teste=0;
                }
              }
              if(teste){
                  vetBuffer[ult_enviado]=teste=vetNumEnviados[i];
                  ierr = MPI_Send(&teste, 1, MPI_INT, proc_raiz, ret_num_primo, MPI_COMM_WORLD);
                  ult_enviado++;
              }else{
              ierr = MPI_Send(&teste, 1, MPI_INT, proc_raiz, ret_num_primo, MPI_COMM_WORLD);
              }
            }
          }
      }//fim do for
      cont++;
    }else{
      //recebe vetor de buffer
      ierr = MPI_Recv( &vetBuffer, sizebuffer, MPI_INT, proc_raiz, envio_vetor, MPI_COMM_WORLD, &status);
      // recebe numreo para ver se é primo
      ierr = MPI_Recv( &num_teste, 1, MPI_INT, proc_raiz, envio_prox_num, MPI_COMM_WORLD, &status);
      // recebe ultimo primo
      ierr = MPI_Recv( &ult_primo, 1, MPI_INT, proc_raiz, envio_tamanho, MPI_COMM_WORLD, &status);
      teste = num_teste;
      for(int i =0; i<ult_primo; i++){
        if(teste % 2 != 0 && teste % primos[i] != 0){
            teste=0;
            break;
        }
      }
      ierr = MPI_Send(&teste, 1, MPI_INT, proc_raiz, ret_num_primo, MPI_COMM_WORLD);

    }
  }
}
    return 0;
}
