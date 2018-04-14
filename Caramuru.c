#include<stdio.h>
#include<stdbool.h>


int tamanho_do_programa_1 = 0;
int tamanho_do_programa_2 = 0;
int tamanho_do_programa_3 = 0;
int tamanho_do_programa_4 = 0;
int tamanho_do_programa_5 = 0;


int verificar_maior_programa(){
	int maior_programa = tamanho_do_programa_1;
	if(maior_programa < tamanho_do_programa_2){
		maior_programa = tamanho_do_programa_2;
	}
	if(maior_programa < tamanho_do_programa_3){
		maior_programa = tamanho_do_programa_3;
	}
	if(maior_programa < tamanho_do_programa_4){
		maior_programa = tamanho_do_programa_4;
	}
	if(maior_programa < tamanho_do_programa_5){
		maior_programa = tamanho_do_programa_5;
	}
	
	return maior_programa;
}

void preenche_linhas(int colunas, int linhas){
	int n_linhas = 1;	
	char executa_p1 = 'O';	
	char executa_p2 = 'O';	
	char executa_p3 = 'O';	
	char executa_p4 = 'O';	
	char executa_p5 = 'O';	


	while(n_linhas <= linhas){
		if(colunas == 1){
			if(tamanho_do_programa_1 >= n_linhas){
				printf("%i	X	\n", n_linhas);				
			}	
		}
		if(colunas == 2){
			if(tamanho_do_programa_1 >= n_linhas){
				executa_p1 = 'X';				
			}	
			else{
				executa_p1 = 'O';				
			}
			if(tamanho_do_programa_2 >= n_linhas){
				executa_p2 = 'X';				
			}	
			else{
				executa_p2 = 'O';				
			}
			printf("%i	%c 	%c	\n", n_linhas, executa_p1, executa_p2);							
		}
		if(colunas == 3){
			if(tamanho_do_programa_1 >= n_linhas){
				executa_p1 = 'X';				
			}	
			else{
				executa_p1 = 'O';				
			}
			if(tamanho_do_programa_2 >= n_linhas){
				executa_p2 = 'X';				
			}	
			else{
				executa_p2 = 'O';				
			}
			if(tamanho_do_programa_3 >= n_linhas){
				executa_p3 = 'X';				
			}	
			else{
				executa_p3 = 'O';				
			}
			printf("%i	%c 	%c	%c\n", n_linhas, executa_p1, executa_p2, executa_p3);							
		}
		if(colunas == 4){
			if(tamanho_do_programa_1 >= n_linhas){
				executa_p1 = 'X';				
			}	
			else{
				executa_p1 = 'O';				
			}
			if(tamanho_do_programa_2 >= n_linhas){
				executa_p2 = 'X';				
			}	
			else{
				executa_p2 = 'O';				
			}
			if(tamanho_do_programa_3 >= n_linhas){
				executa_p3 = 'X';				
			}	
			else{
				executa_p3 = 'O';				
			}
			if(tamanho_do_programa_4 >= n_linhas){
				executa_p4 = 'X';				
			}	
			else{
				executa_p4 = 'O';				
			}
			printf("%i	%c 	%c	%c	%c\n", n_linhas, executa_p1, executa_p2, executa_p3, executa_p4);							
		}
		if(colunas == 5){
			if(tamanho_do_programa_1 >= n_linhas){
				executa_p1 = 'X';				
			}	
			else{
				executa_p1 = 'O';				
			}
			if(tamanho_do_programa_2 >= n_linhas){
				executa_p2 = 'X';				
			}	
			else{
				executa_p2 = 'O';				
			}
			if(tamanho_do_programa_3 >= n_linhas){
				executa_p3 = 'X';				
			}	
			else{
				executa_p3 = 'O';				
			}
			if(tamanho_do_programa_4 >= n_linhas){
				executa_p4 = 'X';				
			}	
			else{
				executa_p4 = 'O';				
			}
			if(tamanho_do_programa_5 >= n_linhas){
				executa_p5 = 'X';				
			}	
			else{
				executa_p5 = 'O';				
			}
			printf("%i	%c 	%c	%c	%c	%c\n", n_linhas, executa_p1, executa_p2, executa_p3, executa_p4, executa_p5);							
		}
		n_linhas++;
	}
}

void processamento_mono(int programas){
	printf("Processamento de monotarefa.\n");				
	if(programas == 1){
		while(tamanho_do_programa_1 > 0){
			printf("A\n");
			tamanho_do_programa_1 = tamanho_do_programa_1 - 1;
		}	
	}
	if(programas == 2){
		while(tamanho_do_programa_1 > 0){
			printf("A\n");
			tamanho_do_programa_1 = tamanho_do_programa_1 - 1;
		}	
		while(tamanho_do_programa_2 > 0){
			printf("B\n");
			tamanho_do_programa_2 = tamanho_do_programa_2 - 1;
		}	
	}

	if(programas == 3){
		while(tamanho_do_programa_1 > 0){
			printf("A\n");
			tamanho_do_programa_1 = tamanho_do_programa_1 - 1;
		}	
		while(tamanho_do_programa_2 > 0){
			printf("B\n");
			tamanho_do_programa_2 = tamanho_do_programa_2 - 1;
		}
		while(tamanho_do_programa_3 > 0){
			printf("C\n");
			tamanho_do_programa_3 = tamanho_do_programa_3 - 1;
		}		
	}

	if(programas == 4){
		while(tamanho_do_programa_1 > 0){
			printf("A\n");
			tamanho_do_programa_1 = tamanho_do_programa_1 - 1;
		}	
		while(tamanho_do_programa_2 > 0){
			printf("B\n");
			tamanho_do_programa_2 = tamanho_do_programa_2 - 1;
		}
		while(tamanho_do_programa_3 > 0){
			printf("C\n");
			tamanho_do_programa_3 = tamanho_do_programa_3 - 1;
		}	
		while(tamanho_do_programa_4 > 0){
			printf("D\n");
			tamanho_do_programa_4 = tamanho_do_programa_4 - 1;
		}		
	}

	if(programas == 5){
		while(tamanho_do_programa_1 > 0){
			printf("A\n");
			tamanho_do_programa_1 = tamanho_do_programa_1 - 1;
		}	
		while(tamanho_do_programa_2 > 0){
			printf("B\n");
			tamanho_do_programa_2 = tamanho_do_programa_2 - 1;
		}
		while(tamanho_do_programa_3 > 0){
			printf("C\n");
			tamanho_do_programa_3 = tamanho_do_programa_3 - 1;
		}	
		while(tamanho_do_programa_4 > 0){
			printf("D\n");
			tamanho_do_programa_4 = tamanho_do_programa_4 - 1;
		}		
		while(tamanho_do_programa_5 > 0){
			printf("E\n");
			tamanho_do_programa_5 = tamanho_do_programa_5 - 1;
		}	
	}
}


void processamento_multi(int programas){
	printf("Processamento de multitarefa.\n");
	if(programas == 1){
		printf("O	A\n");		
	}
	if(programas == 2){
		printf("O	A	B\n");
	}
	if(programas == 3){
		printf("O	A	B	C\n");
	}
	if(programas == 4){
		printf("O	A	B	C	D\n");
	}
	if(programas == 5){
		printf("O	A	B	C	D	E\n");
	}
	preenche_linhas(programas, verificar_maior_programa());
}

void solicita_tamanho(int programas){
	if(programas == 1){
		printf("Informe a o tamanho do programa 1: \n"); 	
		scanf(" %i", &tamanho_do_programa_1);
		tamanho_do_programa_2 = 0;
		tamanho_do_programa_3 = 0;
		tamanho_do_programa_4 = 0;
		tamanho_do_programa_5 = 0;	
	}
	
	if(programas == 2){
		printf("Informe a o tamanho do programa 1: \n"); 	
		scanf(" %i", &tamanho_do_programa_1);
		printf("Informe a o tamanho do programa 2: \n"); 	
		scanf(" %i", &tamanho_do_programa_2);
		tamanho_do_programa_3 = 0;
		tamanho_do_programa_4 = 0;
		tamanho_do_programa_5 = 0;	
	}
			
	if(programas == 3){
		printf("Informe a o tamanho do programa 1: \n"); 	
		scanf(" %i", &tamanho_do_programa_1);
		printf("Informe a o tamanho do programa 2: \n"); 	
		scanf(" %i", &tamanho_do_programa_2);
		printf("Informe a o tamanho do programa 3: \n"); 	
		scanf(" %i", &tamanho_do_programa_3);
		tamanho_do_programa_4 = 0;
		tamanho_do_programa_5 = 0;	
	}

	if(programas == 4){
		printf("Informe a o tamanho do programa 1: \n"); 	
		scanf(" %i", &tamanho_do_programa_1);
		printf("Informe a o tamanho do programa 2: \n"); 	
		scanf(" %i", &tamanho_do_programa_2);
		printf("Informe a o tamanho do programa 3: \n"); 	
		scanf(" %i", &tamanho_do_programa_3);
		printf("Informe a o tamanho do programa 4: \n"); 	
		scanf(" %i", &tamanho_do_programa_4);
		tamanho_do_programa_5 = 0;	
	}
	if(programas == 5){
		printf("Informe a o tamanho do programa 1: \n"); 	
		scanf(" %i", &tamanho_do_programa_1);
		printf("Informe a o tamanho do programa 2: \n"); 	
		scanf(" %i", &tamanho_do_programa_2);
		printf("Informe a o tamanho do programa 3: \n"); 	
		scanf(" %i", &tamanho_do_programa_3);
		printf("Informe a o tamanho do programa 4: \n"); 	
		scanf(" %i", &tamanho_do_programa_4);
		printf("Informe a o tamanho do programa 5: \n"); 	
		scanf(" %i", &tamanho_do_programa_5);
	}
}

int main(){
	int programas_a_analisar = 0;
	int  escolha_tipo_de_so = 0;	

		
	while(true){
		printf("Qual a quantidade de programas de [1 a 5]: \n");
		scanf(" %i", &programas_a_analisar);
		if(programas_a_analisar > 5){
			printf("Valor invalido, informe no intervalo [1 a 5]\n");
		}
		else{		
			printf("A quantidade é: %i\n", programas_a_analisar);
			solicita_tamanho(programas_a_analisar);		
			printf("Informe 1 para Monotarefa e 2 para multitarefa: \n");
			scanf(" %i", &escolha_tipo_de_so);
			if(escolha_tipo_de_so == 1){
				processamento_mono(programas_a_analisar);			
			}
			else if(escolha_tipo_de_so == 2){
				processamento_multi(programas_a_analisar);				
			}
			else{
				printf("Pela escolha errada o programa será finalizado\n");
				break;
			}
		}	
	}
	return 0;
}
