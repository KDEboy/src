
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

/*Considere sistema de loteria que sorteie N números
inteiros positivos únicos e não nulos.
Faça um programa que leia números sorteados e os números de um bilhete qualquer.
Exiba os números sorteados do bilhete em questão.
*/

int main(void){

  int i, j;
	int x;
	int vetor[5] = {0};
  int vetor_usuario[5] = {0};

	/*Sorteio dos números aleatórios*/
  srand( (unsigned)time(NULL) );

	/*Entrada do bilhete do usuário*/
  printf("OBSERVAÇÃO...\n");
  printf("Após digitar um número, pressione enter para digitar o próximo número.\n");
	for(j = 0; j < 5; j++){
		printf("Digite um número(APENAS DENTRO DO INTERVALO[0,9]): ");
		scanf("%d", &vetor_usuario[j]);
	}

  printf("\n");
	printf("A sequencia correta de números soreados é: ");

	/*Printagem da sequencia números aleatórios*/
	for(i = 0 ; i < 5 ; i++){
		vetor[i] = 1 + ( rand() % 9 );
		printf("%d ", vetor[i]);
	}

	printf("\n");
	printf("\n");

	/*Verificação do sorteio*/
	for(x = 0; x < 5; x++){
		if(vetor[x] == vetor_usuario[x]){
			printf("Você acertou o número %d na posição %d.\n", vetor_usuario[x], x+1);
		}
	}
	return 0;
}
