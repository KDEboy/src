
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

/*Considere sistema de loteria que sorteie N números inteiros, 
positivos, únicos e não nulos. Ordene tais números.
Faça um programa que leia números sorteados na loteria e os números de um 
bilhete qualquer. O bilhete obedece os mesmos critérios 
dos números sorteados na loteria.
Armazene e exiba os números sorteados do bilhete em questão.
*/

int main(void){
 
	int nr, nr1, nr2, nr3, nr4;
 	int j;
	int x;
	int vetor[5] = {0};
 	int vetor_usuario[5] = {0};
 	void OrdenaNumeros(int vetor[5]);
	
	/*Sorteio dos números aleatórios*/
 
	srand( (unsigned)time(NULL) );

	/*Geração da sequencia números aleatórios*/
	nr = 1 + (rand() % 9);
	nr1 = 1 + (rand() % 9);
	nr2 = 1 + (rand() % 9);
	nr3 = 1 + (rand() % 9);
	nr4 = 1 + (rand() % 9);

	if(nr == nr1) 
	{
		nr = 1 + (rand() % 9);
	}	
	else if(nr1 == nr2)
	{
		nr1 = 1 + (rand() % 9);
	}	
	else if(nr2 == nr3)
	{
		nr2 = 1 + (rand() % 9);
	}
	else if(nr3 == nr4)
	{
		nr3 = 1 + (rand() % 9);
	}	
	else if(nr4 == nr)
	{
		nr4 = 1 + (rand() % 9);		
	}	

	vetor[0] = nr;
	vetor[1] = nr1;
	vetor[2] = nr2;
	vetor[3] = nr3;
	vetor[4] = nr4;
	
	/*printf("\n");
	printf("\n");
	*/
	/*Entrada do bilhete do usuário*/
	printf("\n");
	printf("OBSERVAÇÃO...\n");
 	printf("Após digitar um número, pressione enter para digitar o próximo número.\n");
	for(j = 0; j < 5; j++){
		printf("Digite um número(APENAS DENTRO DO INTERVALO[0,9]): ");
		scanf("%d", &vetor_usuario[j]);

	}

	printf("\n");
  	printf("\n");
	printf("A sequencia sorteada é: ");
	OrdenaNumeros(vetor);
	printf("\n");

	/*Verificação do sorteio*/
	for(x = 0; x < 5; x++){
		if(vetor[0] == vetor_usuario[x]){
			printf("Você acertou o número %d.\n", vetor_usuario[x]);
		}
		else if(vetor[1] == vetor_usuario[x]){
			printf("Você acertou o número %d.\n", vetor_usuario[x]);
		}		
		else if(vetor[2] == vetor_usuario[x]){
			printf("Você acertou o número %d.\n", vetor_usuario[x]);
		}		
		else if(vetor[3] == vetor_usuario[x]){
			printf("Você acertou o número %d.\n", vetor_usuario[x]);
		}		
		else if(vetor[4] == vetor_usuario[x]){
			printf("Você acertou o número %d.\n", vetor_usuario[x]);
		}				
	}
	return 0;
}

void OrdenaNumeros(int vetor[5])
{

	int aux = 0;
	int i = 0;
	int j = 0;
	int k = 0;
	
	for(i = 0; i < 5; i++)
	{
		for(j = 0; j < 5; j++)
		{
			if(vetor[i] < vetor[j])
			{
				aux = vetor[i];
				vetor[i] = vetor[j];
				vetor[j] = aux;
			}
		}
	}			

	for(k = 0; k < 5; k++)
	{
		printf(" %d", vetor[k]);
	}	
}























