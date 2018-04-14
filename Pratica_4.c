#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

void imprime_frase(int f){
		if (f == 1){
			printf("Frase 1\n");
		}
		else if (f == 2){
			printf("Frase 2\n");
		}
		else if (f == 3){
			printf("Frase 3\n");
		}
		else if (f == 4){
			printf("Frase 4\n");
		}
		else if (f == 5){
			printf("Frase 5\n");
		}
  		else if (f == 6){
			printf("Frase 6\n");
		}
		else if (f == 7){
			printf("Frase 7\n");
		}
		else if (f == 8){
			printf("Frase 8\n");
		}
		else if (f == 9){
			printf("Frase 9\n");
		}
	}

void sorteio_frase(){		
  	imprime_frase((rand() % 9) + 1);
}

int main(){
	bool v = false;
	char c;
	while(true)
	{
		printf("Digite a entrada: ");
		scanf(" %c", &c);
		if(c == 'x'){		
			printf("Saindo...\n");			
			break;	
		}			
		else if(c == 'h'){
			printf("1- Entrada for um inteiro entre 1 e 9. FRASES PARADOXAIS E CONTRADITORIAS\n");
 			printf("2- Entrada igual a 0. FRASES NORMAIS\n");
 			printf("3- Entrada igual a x. SAI DO PROGRAMA\n");
		}	
		else if(c == '1'){
			imprime_frase(1);
		}	
		else if(c == '2'){
			imprime_frase(2);
		}
		else if(c == '3'){
			imprime_frase(3);
		}
		else if(c == '4'){
			imprime_frase(4);
		}
		else if(c == '5'){
			imprime_frase(5);
		}
		else if(c == '6'){
			imprime_frase(6);
		}
		else if(c == '7'){
			imprime_frase(7);
		}
		else if(c == '8'){
			imprime_frase(8);
		}
		else if(c == '9'){
			imprime_frase(9);
		}
		else if(c == '0'){
			sorteio_frase();
		}
		else{
			if(v == false){		
				printf("Comando incorreto, utilize h para ajuda\n");
				v = true;			
			}			
		}	
	}		
	return 0;
}

























