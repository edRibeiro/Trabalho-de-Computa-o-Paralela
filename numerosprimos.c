// Programa Paralelo para cálculo de números primos
// Disciplina: Programação Paralela
// Prof. Francisco (Kiko)
// Aluno: Ederson Ribeiro Silva
// Data: 30/05/17

//Para o melhor desenvolvimento do paralelismo, utilizei a técnica de envio "Buffered" padrão do MPI_SEND

#include <stdio.h>
#include <mpi.h>
#include <time.h>

#define max_lin 10000000
//Essas definições são tags para marcar o envio e recebimento de mensagens
#define envio_prox_num 2001
#define ret_num_primo 2002
#define envio_vetor 3001
#define envio_tamanho 4001
#define proc_raiz 0


int primos[max_lin];
clock_t inicioTempo, fimTempo;

main(int argc, char **argv){
  int num_primo; //Número primo enviado pelos processos paralelos
	int size; 	/* Último número que será verificado */
	MPI_Status status; //Definindo a variável "status"
	int ierr, my_id, num_procs; //Definições para o paralelismo
  int verificou;
	/* Deste ponto em diante, este processo cria processos paralelos com cópias deste programa.*/
	ierr = MPI_Init(&argc, &argv);
	/* Encontrando o ID do processo e quantos processos foram criados */
      	/* find out MY process ID, and how many processes were started. */
      	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
      	ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    verificou = 0;
    //Garantindo que todo o vetor tenha valor 0 em suas posições
    for(size=0; size<max_lin; size++){
        primos[size]=0;
    }
    primos[0]=1; 	/* primeiro número primo */

    //int vetNumerosEnviados[num_procs]; //VETOR QUE FICARÁ RESPONSÁVEL POR ENVIAR OS VALORES QUE ESTAO NOS OUTROS PROCESSOS

	if (my_id == proc_raiz){
    do{
		printf("Serao analisados os números primos entre 1 e o valor digitado pelo usuario\n");
		printf("Qual o ultimo numero do intervalo a ser verificado? ");
        	scanf("%i", &size); /* Último número do intervalo de primos */

		if ((size <= 1)||(size > max_lin)){ /* Número Inválido */
			printf("Valor digitado pelo usuario e invalido!");
		}}
    while((size <= 1)||(size > max_lin));

		/* Marcando o início do relógio */
		inicioTempo= clock();

		int num_teste = 2; //Número atual a ser testado
		int pos = 1; //Posição de teste
		int ult_primo = 0; //Posição do último número primo encontrado
    int ult_enviado = 0; //Posição do ultimo valor enviado
    int posicao; //CONTROLAR A POSICAO EM QUE SE ENCONTRA O MEU VETOR COM OS NÚMEROS A SEREM DIVIDIDOS
    int aux;//VARIAVEL QUE IRA RECEBER O VALOR DO NUMERO QUE SERA ENVIADO
    int vetNumerosEnviados[num_procs];
		//Enviando o próximo número para os processos escravos criados
		int processo=0;

		//Cada processo receberá o próximo número que será calculado
		for (num_teste=2; num_teste<=size; num_teste++){
			processo++;
      aux = num_teste + num_procs - 1;

      //LOOP PARA PREENCHER O VETOR QUE IRÁ CONTER OS VALORES QUE ESTÃO SENDO DIVIDIDOS PELOS OUTROS PROCESSOS
      //ELE IRÁ EXECUTAR SEMPRE QUE O CICLO DE ENVIO REINICIAR
      if(num_teste < (num_procs*2)){
        if (processo == 1){
          ult_enviado = 0;
          for ( posicao = 0; posicao < num_procs; posicao++){
            ult_enviado++;

            vetNumerosEnviados[posicao] = aux;
            if (aux < 0 ){
              vetNumerosEnviados[posicao] = 0;
            }
            aux = aux - 1;
          }
        }
      }

      ierr = MPI_Send(&ult_enviado, 1, MPI_INT, processo, envio_tamanho, MPI_COMM_WORLD);

      ierr = MPI_Send(&vetNumerosEnviados[0], ult_enviado , MPI_INT, processo, envio_vetor, MPI_COMM_WORLD);

            //printf("Enviando %d para o processo %d  -> ", num_teste, processo);
			ierr = MPI_Send(&num_teste, 1 , MPI_INT, processo, envio_prox_num, MPI_COMM_WORLD);

			if(processo >= num_procs-1){ //Neste caso todos os processos já receberam um número
                                          //é hora de pegar os resultados
                                          //Como foi feito pode gerar DEADLOCK!!
                                          //ou pode deixar de incluir alguns números primos!!
                for(processo=1; processo<num_procs; processo++){
                    ierr = MPI_Recv( &num_primo, 1, MPI_INT, MPI_ANY_SOURCE, ret_num_primo, MPI_COMM_WORLD, &status);
                    if(num_primo != 0){ //Trata-se de um número primo
                        ult_primo++;
                        primos[ult_primo]=num_primo; //Adicionando o número primo ao vetor de primos
                    }
                }
                for(processo=1; processo<num_procs; processo ++){ // Enviando o tamanho do vetor de primos
                    ierr = MPI_Send(&ult_primo, 1, MPI_INT, processo, envio_tamanho, MPI_COMM_WORLD);
                }
                for(processo=1; processo<num_procs; processo ++){ // Hora de enviar aos processos escravos
                                                                  // o novo vetor de números primos
                    ierr = MPI_Send(&primos[0], ult_primo, MPI_INT, processo, envio_vetor, MPI_COMM_WORLD);
                }
                processo=0;
      }
		}

       //Finalmente é hora de exibir os resultados do Vetor!
	   //Exibindo os resultado
	   for (pos=0; pos<=ult_primo; pos++)
       printf("%d ", primos[pos]);
	     printf("\n");
	     printf("Quantidade de primos encontrados: %d", ult_primo+1);

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
    }
    else{ //Chegou a hora de programar os processos escravos
        int num_teste;
        int pos = 1; //Posição de teste
        int primo = 0; //Inicialmente todos os números são primos
        int ult_primo = 1;
        int ult_enviado = 1;
        int posicaoEnviado = 1;
        int vetNumerosEnviados[num_procs];
        while(1){
            pos=1;
            posicaoEnviado = 1;

            //Recebendo o tamanho do vetor de ENVIADOS
            ierr = MPI_Recv( &ult_enviado, 1, MPI_INT, proc_raiz, envio_tamanho, MPI_COMM_WORLD, &status);

            //RECEBENDO OS NÚMEROS QUE ESTÃO SENDO DIVIDIDOS NOS OUTROS PROCESSOS
            ierr = MPI_Recv( &vetNumerosEnviados[0], ult_enviado, MPI_INT, proc_raiz, envio_vetor, MPI_COMM_WORLD, &status);


            //Recebendo os números do processo Raiz para testar
            ierr = MPI_Recv( &num_teste, 1, MPI_INT, proc_raiz, envio_prox_num, MPI_COMM_WORLD, &status);

            //printf("Recebido o valor %i no processo %i\n", num_teste, my_id);
            int primo = 0; //Inicialmente todos os números são primos
            //if(num_teste < (num_procs-1)){ //Neste caso faremos o teste para o número primo com
            if (verificou == 0) {                               //todos os números menores que ele

                for ( pos = 0; pos < ult_enviado; pos++){ //Rotina para testar se o número é primo ou não com os numeros que estão no outro processo
                  if (vetNumerosEnviados[pos] < num_teste){
                    if((num_teste % vetNumerosEnviados[pos])==0) primo=1; // Se der resto 0 não é primo
                  }
                }
                verificou = 1;
        //        for(pos=2;pos<num_teste;pos++)
        //            if((num_teste % pos)==0) primo=1;
            }else{
                while(primos[pos]!=0){ //Rotina para testar se o número é primo ou não
                    if((num_teste % primos[pos])==0) primo=1; // Se der resto 0 não é primo
                    pos++;
                }
            }

            if(primo==1) num_teste = 0; // O número não é primo, portanto retornarei 0
            //Enviando o primo para o processo raiz
            ierr = MPI_Send(&num_teste, 1, MPI_INT, proc_raiz, ret_num_primo, MPI_COMM_WORLD);

            //Recebendo o tamanho do vetor de primos
            ierr = MPI_Recv( &ult_primo, 1, MPI_INT, proc_raiz, envio_tamanho, MPI_COMM_WORLD, &status);
            //Recebendo o novo vetor com os primos calculados
            ierr = MPI_Recv( &primos[0], ult_primo, MPI_INT, proc_raiz, envio_vetor, MPI_COMM_WORLD, &status);
        }
    }
}
