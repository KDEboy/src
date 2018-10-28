#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 500

int main(void){
    char *perguntas[N] = {"O personagem é real?", "É Brasileiro(a)?", "É cantor(a)?", "É homem?"};
    char escolha = '?';

    printf("\n\n");
    printf("AKINATOR feito por Daniel\n");
    printf("A lista de personagens é composta por:\nMario\nZelda\nSandy\nJunior\nRodrigo Santoro\n"
"Juliana Paes\nAmy Lee\nJames Hetfield\nDaniel Creig\nScarlett Johansson\n");    
    printf("\n\n");
    printf("Para mais informações sobre os personagens acesse os links abaixo.\n");
    printf("Mario --- https://pt.wikipedia.org/wiki/Mario_(personagem)\n"
            "Zelda --- https://pt.wikipedia.org/wiki/The_Legend_of_Zelda_(série)\n"
            "Sandy --- https://pt.wikipedia.org/wiki/Sandy\n"
            "Junior --- https://pt.wikipedia.org/wiki/Junior_Lima\n"
            "Rodrigo Santoro --- https://pt.wikipedia.org/wiki/Rodrigo_Santoro\n"
            "Juliana Paes --- https://pt.wikipedia.org/wiki/Juliana_Paes\n"
            "Amy Lee --- https://pt.wikipedia.org/wiki/Amy_Lee\n"
            "James Hetfield --- https://pt.wikipedia.org/wiki/James_Hetfield\n"
            "Daniel Craig --- https://pt.wikipedia.org/wiki/Daniel_Craig\n"
            "Scarlett Johansson --- https://pt.wikipedia.org/wiki/Scarlett_Johansson\n");
    printf("\n\n");
    printf("Iniciando Akinator versão 0.1 beta ...");
    printf("\n");
    printf("NOTA: DIGITE APENAS 's' PARA SIM OU 'n' PARA NÃO ... \n");



    printf("%s\n", perguntas[0]);
    scanf(" %c", &escolha);

    if(escolha == 's'){
        printf("%s\n", perguntas[1]);
        scanf(" %c", &escolha);
        if(escolha == 's'){
            printf("%s\n", perguntas[2]);
            scanf(" %c", &escolha);
            if(escolha == 's'){
                printf("%s\n", perguntas[3]);
                scanf(" %c", &escolha);        
                if(escolha == 's'){
                    printf("É o Junior\n");

                }
                else{
                    printf("É a Sandy\n");
                }
            }
            else{
                printf("%s\n", perguntas[3]);
                scanf(" %c", &escolha);
                if(escolha == 's'){
                    printf("É o Rodrigo Santoro\n");
                }
                else{
                    printf("É a Juliana Paes\n");
                }
            }               
        }
        else{
            printf("%s\n", perguntas[2]);
            scanf(" %c", &escolha);
            if(escolha == 's'){
                printf("%s\n", perguntas[3]);
                scanf(" %c", &escolha);        
                if(escolha == 's'){
                    printf("É o James Hetfield\n");

                }
                else{
                    printf("É a Amy Lee.\n");
                }
            }  
            else{
                printf("%s\n", perguntas[3]);
                scanf(" %c", &escolha);
                if(escolha == 's'){
                    printf("É o Daniel Craig.\n");
                }
                else{
                    printf("É a Scarlett Johansson.\n");
                }
            }             
        }        
    }
    else{
        printf("%s\n", perguntas[3]);
        scanf(" %c", &escolha);
        if(escolha == 's'){
            printf("É o mario.\n");
        }
        else{
            printf("É a Zelda.\n");
        }
    }
    return 0;
}

