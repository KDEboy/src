
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int main(void){

  int CalculaFibo(int x);
  int num = 0;
  int fibo = 0;

  char a;

  printf("Escolha r para Recursiva e i para Iterativa: ");
  scanf("%c", &a);

  if(a == 'r'){
      printf("Digite o número que você quer saber o Fibonacci correspondente: ");
      scanf("%d", &num);

      fibo = CalculaFibo(num);
      printf("Teu Fibonnaci é %d", fibo);
  }
  else if(a == 'i'){
    printf("Em contrução...(Aceito Dicas)");
  }
  return 0;
}

/*Função Recursiva para calcular Fibonnaci.*/
int CalculaFibo(int x){
/* Formula geral: Fn = Fn - 1 + Fn - 2*/
  if(x == 0){
    return 0;
  }
  else if(x == 1){
    return 1;
  }

  return CalculaFibo(x - 1) + CalculaFibo(x - 2);
}
