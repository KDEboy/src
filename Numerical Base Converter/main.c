
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

void Binario_para_Decimal();
void Decimal_para_Binario();
void Decimal_para_Hexadecimal();
void Hexacimal_para_Decimal();
void Decimal_para_Octal();
void Octal_para_Decimal();

int main(void){
    char escolha = '?';

    printf("\n");
    printf("Conversor de bases (DECIMAL -> QUALQUER BASE E QUALQUER BASE -> DECIMAL)\n\n");
    printf("Digite:\n=> 'b' para converter um binário para decimal.\n"
	"=> 'd' para converter um decimal para a base binária.\n"
	"=> 'h' para converter um decimal para hexadecimal.\n"
	"=> 'x' para converter um hexadecimal para decimal.\n"
	"=> 'o' para converter um octal pra decimal.\n"
	"=> 'c' para converter um decimal em octal.\n");
	printf("\nESCOLHA => ");
    scanf("%c", &escolha);

    switch(escolha){
        case 'b':
            Binario_para_Decimal();
            break;
        case 'd':
            Decimal_para_Binario();
            break;
        case 'h':
        	Decimal_para_Hexadecimal();
        	break;
        case 'x':
        	Hexacimal_para_Decimal();
        	break;
        case 'o':
			Decimal_para_Octal();
        	break;
        case 'c':
			Octal_para_Decimal();        	
			break;
        default:
            printf("Você escolheu sair ... Teu computador vai ser desligado em 1 minuto :v\n");
            break;       
    }
    return 0;
}

void Binario_para_Decimal(){
    int vetor[80] = {0};
    int i = 0;
	int j = 0;
	int k = 0;
	
	printf("LEMBRE-SE APERTE ENTER A CADA NÚMERO E QUALQUER COISA DIFERENTE DE 1 OU 0 PARA SAIR...\n\n");
    printf("Informe um número binário pra converter para decimal\n");

	for(i = 0; i < 80; i++){	
        int count = 0;
		printf("NUMERO => ");        		
		scanf(" %d", &count);
		
		if (count != 0 && count != 1){
			break;
		}
        vetor[i] = count;
	}    
    //printf("\n i:%d\n ", i);
    for(j = 0; j < i; j++){
        //printf(" %d", vetor[j]);
    }
    int total = 0;
	
    for(k = 0; k < j; k++){
		if(vetor[k] == 1){
			vetor[k] *= pow(2, k);	
		}	
		
		total += vetor[k];
	}	
    
	printf("\nO Binário convertido para decimal é: %d\n", total);
}

void Decimal_para_Binario(){	
	int escolha_user = 0;

	printf("Nota: O binário deve ser lido de trás pra frente ... Infelizmente não consegui arrumar isso .-\n\n");
    printf("Informe um número decimal pra converter para binário\n");
	printf("NUMERO => ");    		
	scanf(" %d", &escolha_user);
	
	while(escolha_user != 0){
	  printf("%d", escolha_user & 1);
	  escolha_user = escolha_user >> 1;
	}
	printf("\n");	
}

void Decimal_para_Hexadecimal(){
    int numeroEntrada, q, resto, i;
	char hexa[10] = {0}, getInt[10] = {0};
	
	printf("Informe um numero decimal para converter para hexadecimal\n");
	printf("NUMERO => ");
	scanf("%i", &numeroEntrada);
	
	do{
		q = numeroEntrada / 16;
		resto = numeroEntrada % 16;
		numeroEntrada /= 16;
		
		switch(resto){
			case 10:
				strcat(hexa, "A");
			break;
			case 11:
				strcat(hexa, "B");
			break;
			case 12:
				strcat(hexa, "C");
			break;
			case 13:
				strcat(hexa, "D");
			break;
			case 14:
				strcat(hexa, "E");
			break;
			case 15:
				strcat(hexa, "F");
			break;
			case 16:
				strcat(hexa, "G");
			break;
			default:
				sprintf(getInt, "%i", resto);
				strcat(hexa, getInt);
		}
	}while(q != 0);
	
	for(i=strlen(hexa); i >= 0; i--){
		printf("%c", hexa[i]);
	}
	printf("\n");
}

void Hexacimal_para_Decimal(){
	int vetor[80];
    int i = 0, j = 0, total = 0, tam = 0, count = 0;
	
	printf("NÃO INSIRA LETRAS, APENAS NÚMEROS... Exemplo, em vez de inserir F, insira 15 ...\n");
	printf("LEMBRE-SE APERTE ENTER A CADA NÚMERO E -1 PARA SAIR...\n\n");
    printf("Informe um número hexadecimal pra converter para decimal\n");

	for(i = 0; i < 80; i++){	
        printf("NUMERO => ");		
		scanf("%d", &count);

		if(count == -1){
			break;
		}
        vetor[i] = count;
	}    
    //printf("\n i:%d\n ", i);
 	tam = i-1;
   
    for(j = 0; j < i; j++){
        //printf(" %d", vetor[j]);
    }

	for(i = 0; i <= tam; i++){
		vetor[i] *= pow(16, tam-i);	
		total += vetor[i];
	}	
    
	printf("\nO decimal é: %d\n", total);
}

void Decimal_para_Octal(){
    long int octal, decimal = 0;
    int i = 0;
 
    printf("Informe um número octal para converter para decimal\n");
    printf("NUMERO => ");
    scanf("%ld", &octal);
    while (octal != 0){
        decimal =  decimal +(octal % 10)* pow(8, i++);
        octal = octal / 10;
    }
    printf("O decimal é: %ld\n",decimal);
}

void Octal_para_Decimal(){
	long decimal_num, quociente;
    int octalNumber[100], i = 1, j;
 
    printf("Informe um número decimal para converter para octal\n");
    printf("NUMERO => ");
    scanf("%ld", &decimal_num);
    quociente = decimal_num;
    while (quociente != 0)
    {
        octalNumber[i++] = quociente % 8;
        quociente = quociente / 8;
    }
    printf("\n");
    printf("O octal é: ");
    for (j = i - 1; j > 0; j--){
        printf("%d", octalNumber[j]);
	}
    printf("\n");
}