
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

int main(void)
{
 	int j = 0, x = 0, i = 0;
	bool vetor[5] = {0};
	int vetor_novo[6] = {0};
 	int vetor_usuario[5] = {0};
 	void OrdenaNumeros(int vetor_novo[6]);
	

	/*Entrada do bilhete do usuário*/
	printf("\n");
	printf("OBSERVAÇÃO...\n");
 	printf("Após digitar um número, pressione enter para digitar o próximo número.\n");
	for(j = 0; j < 5; j++)
	{
		printf("Digite um número(APENAS DENTRO DO INTERVALO[0,9]): ");
		scanf("%d", &vetor_usuario[j]);
	}

	/*Geração da sequencia números aleatórios*/
	printf("\n");
	printf("Log de saída dos números aleatórios ... \n");

	/*Semente dos números aleatórios*/
	srand( (unsigned)time(NULL) );

	for(i = 0; i < 5; ++i)
	{
		int r = 1 + (rand() % 9);
		vetor_novo[i] = r;
		if(!vetor[r])
		{	
			printf("Número %d gerado com sucesso .\n", r);
		}
		else
		{
			i--;
			vetor[r]=1;
		}    
	  }

  	printf("\n");
	printf("A sequencia total do sorteio é: ");
	OrdenaNumeros(vetor_novo);
	printf("\n");

	/*Verificação do sorteio*/
	printf("\n");
	printf("POR FAVOR AGUARDE ALGUNS SEGUNDOS PARA A VALIDAÇÃO DO BILHETE ... \n");
	printf("\n");
	for(x = 0; x < 5; x++)
	{
		if(vetor_novo[0] == vetor_usuario[x])
		{
			printf("P-A-R-A-B-É-N-S ! ! ! Você acertou o número %d.\n", vetor_usuario[x]);
		}
		else if(vetor_novo[1] == vetor_usuario[x])
		{
			printf("P-A-R-A-B-É-N-S ! ! ! Você acertou o número %d.\n", vetor_usuario[x]);
		}		
		else if(vetor_novo[2] == vetor_usuario[x])
		{
			printf("P-A-R-A-B-É-N-S ! ! ! Você acertou o número %d.\n", vetor_usuario[x]);
		}		
		else if(vetor_novo[3] == vetor_usuario[x])
		{
			printf("P-A-R-A-B-É-N-S ! ! ! Você acertou o número %d.\n", vetor_usuario[x]);
		}		
		else if(vetor_novo[4] == vetor_usuario[x])
		{
			printf("P-A-R-A-B-É-N-S ! ! ! Você acertou o número %d.\n", vetor_usuario[x]);
		}				
	}
	return 0;
}

void OrdenaNumeros(int vetor_novo[6])
{
	int aux = 0;
	int i = 0;
	int j = 0;
	int k = 0;
	
	for(i = 0; i < 5; i++)
	{
		for(j = 0; j < 5; j++)
		{
			if(vetor_novo[i] < vetor_novo[j])
			{
				aux = vetor_novo[i];
				vetor_novo[i] = vetor_novo[j];
				vetor_novo[j] = aux;
			}
		}
	}			

	for(k = 0; k < 5; k++)
	{
		printf(" %d", vetor_novo[k]);
	}	
}























