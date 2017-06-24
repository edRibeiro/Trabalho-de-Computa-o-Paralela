#include <stdio.h>
#include <stdlib.h>
// #include <conio.h>
#include <mpi.h>

#define root_process 0
#define send_data_tag 2001
#define return_data_tag 2002

int main(int argc, char **argv)
{
    float v[100], v2[100], v3[100]; //definindo o ponteiro v
    int i, j, aux, num_componentes;


    // MPI_Status status;
    int my_id, ierr, num_procs, num_rows,
    an_id, num_rows_to_receive, avg_rows_per_process,
    sender, num_rows_received, start_row, end_row, num_rows_to_send, rank; //Vari�veis para a Bib.MPI

    ierr = MPI_Init(&argc, &argv); //Inicio do MPI

    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if(my_id == 0)
    {
      printf("Informe o numero de componentes do vetor\n");
      scanf("%i", &num_componentes);


      // v = (float *) malloc(num_componentes * sizeof(float));
      // v2 = (float *) malloc(num_componentes * sizeof(float));
      // v3 = (float *) malloc(num_componentes * sizeof(float));

      //Armazenando os dados em um vetor
      for (i = 0; i < num_componentes; i++)
      {
        printf("\nDigite o valor para a posicao %d do vetor: ", i+1);
        scanf("%f",&v[i]);
      }

      avg_rows_per_process = num_componentes / num_procs;

      printf("\n Quantidade de componentes por processo: %d", avg_rows_per_process);

      /* Distribuindo uma parte do vetor para cada processo */

        for(an_id = 1; an_id < num_procs; an_id++) {
            printf("\n ID %d de %d processos \n", an_id, num_procs);

            start_row = an_id*avg_rows_per_process + 1;
            end_row   = (an_id + 1)*avg_rows_per_process;

            printf("\nProcesso %d - comeca da linha %d \n", an_id, start_row);

            if((num_componentes - end_row) < avg_rows_per_process)
               end_row = num_componentes - 1;

             printf("\nProcesso %d - termina da linha %d \n", an_id, end_row);

            num_rows_to_send = end_row - start_row + 1;

            ierr = MPI_Send( &num_rows_to_send, 1 , MPI_INT,
                  an_id, send_data_tag, MPI_COMM_WORLD);

            ierr = MPI_Send( &v[start_row], num_rows_to_send, MPI_FLOAT,
                  an_id, send_data_tag, MPI_COMM_WORLD);


         }

      for( i = 0; i < avg_rows_per_process + 1; i++ )
      {
        v3[i] = v[i];
      }

      for( i = 0; i < avg_rows_per_process + 1; i++ )
      {
        for( j = i + 1; j < avg_rows_per_process; j++ ) // sempre 1 elemento � frente
        {
          // se o (i > (i+1)) ent�o o i passa pra frente (ordem crescente)
          if ( v3[i] > v3[j] )
          {
             aux = v3[i];
             v3[i] = v3[j];
             v3[j] = aux;
          }
        }
      }

      for(an_id = 1; an_id < num_procs; an_id++) {
        start_row = an_id*avg_rows_per_process + 1;
        end_row   = (an_id + 1)*avg_rows_per_process;

        if((num_componentes - end_row) < avg_rows_per_process)
           end_row = num_componentes - 1;

        num_rows_to_send = end_row - start_row + 1;

        ierr = MPI_Recv( &v3[start_row], num_rows_to_send, MPI_FLOAT, MPI_ANY_SOURCE,
              return_data_tag, MPI_COMM_WORLD/*, &status*/);

        // sender = status.MPI_SOURCE;


      }

      for( i = 0; i < num_componentes; i++ )
      {
        for( j = i + 1; j < num_componentes; j++ ) // sempre 1 elemento � frente
        {
          // se o (i > (i+1)) ent�o o i passa pra frente (ordem crescente)
          if ( v3[i] > v3[j] )
          {
             aux = v3[i];
             v3[i] = v3[j];
             v3[j] = aux;
          }
        }
      }

      // ------ Percorrendo o vetor e imprimindo os valores ----------
      printf("\n*********** Valores do vetor dinamico ************\n\n");

      for (i = 0;i < num_componentes; i++)
      {
         printf("%.2f\n",v3[i]);
      }


       //liberando o espa�o de mem�ria alocado
       // free(v);
    }

    else{

        printf("Inicializando processo %d \n", my_id);
       /* Este c�digo � para os processos filhos, portanto devem receber o segmento do vetor gravando em um vetor local */

         ierr = MPI_Recv( &num_rows_to_receive, 1, MPI_INT,
               root_process, send_data_tag, MPI_COMM_WORLD/*, &status*/);

         ierr = MPI_Recv( &v2, num_rows_to_receive, MPI_FLOAT,
               root_process, send_data_tag, MPI_COMM_WORLD/*, &status*/);

         num_rows_received = num_rows_to_receive;

         /*Calculando o somatorio da parte do vetor */

      for( i = 0; i < num_rows_received; i++ )
      {
        for( j = i + 1; j < num_rows_received; j++ ) // sempre 1 elemento � frente
        {
          // se o (i > (i+1)) ent�o o i passa pra frente (ordem crescente)
          if ( v2[i] > v2[j] )
          {
             aux = v2[i];
             v2[i] = v2[j];
             v2[j] = aux;
          }
        }
      }

    /* Finalmente encaminhando a soma parcial para o processo raiz */

     ierr = MPI_Send( &v2, num_rows_received, MPI_FLOAT, root_process,
           return_data_tag, MPI_COMM_WORLD);
    }

   ierr = MPI_Finalize();
   return 0;
}
