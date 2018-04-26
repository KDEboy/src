/*entrada -> numero (q é o lado de um losango)*/

#include <stdio.h>
#include <stdbool.h>

int main(){
	int lado = 0;

	printf("Digite o lado do losango: ");
	scanf(" %i", &lado);

	int i = 0;
	int s = 0;
	
	
	for(i = 1; i <= lado; i++){
		for(s = 1; s <= lado - i; s++){
			printf(" ");
		}
		printf(".");
		
		for(s = lado - i + 1; s < lado; s++){
			printf("  ");
		}
		printf(".");
		
		printf("\n");
	}
	
		
	for(i = 1; i <= lado; i++){
		for(s = lado - i + 1; s < lado; s++){
			printf(" ");
		}
		printf(".");
		
		for(s = 1; s <= lado - i; s++){
			printf("  ");
		}
		printf(".");
		
		printf("\n");
	}
	
	return 0;
}
