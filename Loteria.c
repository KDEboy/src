
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

/*##################################################################################################################################################################*/

int main(void)
{
 	int j = 0, x = 0, i = 0, r = 0;
	bool vetor[5] = {0};
	int vetor_novo[5] = {0};
 	int vetor_usuario[5] = {0};

	void OrdenaNumeros(int vetor_novo[5]);
	void Gera_Aleatorios(int i, int r, int vetor_novo[5], bool vetor[5])	;
	void Usuario(int j, int vetor_usuario[5]);
	void Verifica_Resultado(int vetor_novo[5], int vetor_usuario[5], int x);
	void 	Imprime_Bilhetes(int vetor_novo[5], int vetor_usuario[5]);

	Usuario(j, vetor_usuario);
	Gera_Aleatorios(i, r, vetor_novo, vetor)	;

	printf("\n");
	OrdenaNumeros(vetor_novo);

	Imprime_Bilhetes(vetor_novo, vetor_usuario);

	printf("\n");
	Verifica_Resultado(vetor_novo, vetor_usuario, x);

	return 0;
}

/*##################################################################################################################################################################*/

/*FUNÇÃO DO USUARIO*/
void Usuario(int j, int vetor_usuario[5])
{
	printf("\n");
	printf("OBSERVAÇÃO...\n");
 	printf("Após digitar um número, pressione enter para digitar o próximo número.\n");
	for(j = 0; j < 5; j++)
	{
		printf("Digite um número(APENAS DENTRO DO INTERVALO[0,9]): ");
		scanf("%d", &vetor_usuario[j]);
	}
}
/*FUNÇÃO PARA GERAR NÚMEROS ALEATORIOS UNICOS --- AINDA NÃO FUNCIONANDO TOTALMENTE*/
void Gera_Aleatorios(int i, int r, int vetor_novo[5], bool vetor[5])
{
	printf("\n");
	printf("LOG DA GERAÇÃO DOS NÚMEROS ALEATÓRIOS ...\n");
	printf("\n");
	vetor_novo[0] = r;
	srand( (unsigned)time(NULL) );
	for(i = 0; i < 5; ++i)
	{
		r = 1 + (rand() % 9);
		vetor_novo[i] = r;
		if(!vetor[r])
		{
		  printf("Número %d gerado com sucesso ...\n", r);
		}
		else
		{
		  i--;
		}
		vetor[r] = 1;    
	  }
}
/*FUNÇÃO PARA ORDENAR NÚMEROS ALEATÓRIOS*/
void OrdenaNumeros(int vetor_novo[5])
{
	int aux = 0;
	int i = 0;
	int j = 0;
	
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
}
/*FUNÇÃO PARA VERIFICAR O RESULTADO*/
void Verifica_Resultado(int vetor_novo[5], int vetor_usuario[5], int x)
{
	printf("POR FAVOR AGUARDE ALGUNS SEGUNDOS PARA A VALIDAÇÃO DO BILHETE ... \n");
	printf("\n");
	for(x = 0; x < 5; x++)
	{
		if(vetor_novo[0] != vetor_usuario[x])
		{
			printf("P-A-R-A-B-É-N-S ! ! ! Você NÃO acertou o número %d.\n", vetor_usuario[x]);
		}
		else if(vetor_novo[1] != vetor_usuario[x])
		{
			printf("P-A-R-A-B-É-N-S ! ! ! Você NÃO acertou o número %d.\n", vetor_usuario[x]);
		}		
		else if(vetor_novo[2] != vetor_usuario[x])
		{
			printf("P-A-R-A-B-É-N-S ! ! ! Você NÃO acertou o número %d.\n", vetor_usuario[x]);
		}		
		else if(vetor_novo[3] != vetor_usuario[x])
		{
			printf("P-A-R-A-B-É-N-S ! ! ! Você NÃO acertou o número %d.\n", vetor_usuario[x]);
		}		
		else if(vetor_novo[4] != vetor_usuario[x])
		{
			printf("P-A-R-A-B-É-N-S ! ! ! Você NÃO acertou o número %d.\n", vetor_usuario[x]);
		}				
	}
}

void 	Imprime_Bilhetes(int vetor_novo[5], int vetor_usuario[5])
{
	int aleatorio, usuario;
	printf("O bilhete aleatorio é: ");
	for(aleatorio = 0;  aleatorio < 5;  aleatorio++)
	{
		printf("%d", vetor_novo[aleatorio]);
	}
	printf("\n");
	printf("O bilhete do usuario é: ");
	for(usuario = 0;  usuario < 5;  usuario++)
	{
		printf("%d", vetor_usuario[usuario]);
	}
	printf("\n");
}
/*##################################################################################################################################################################*/


















