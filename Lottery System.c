
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int main(void)
{
    int validateInput = 0;
    char choice;

    srand((unsigned)time(NULL));

    void Quina(int validateInput);
    void Timemania(int validateInput);
    void Lotofacil(int validateInput);
    void Megasena(int validateInput);
    void Dupla_Sena(int validateInput);
    void DiaDeSorte(int validateInput);
    void Lotomania(int validateInput);
    void Federal();
    
    printf("LOTTERY SYSTEM VERSION 0.1 ALPHA.\n");
    printf("A system to test your luck and still, if you want, you can use the random sequence generated in the real draws.\n");
    printf("\n");
    printf("Note...\n");
    printf("After entering a number, press enter to enter the next number.\n");

    printf("Type 'q' for Quina,'t' for Timemania, 'l' for Lotofacil, 'm' for Megasena, "
    "'d' for Dupla-Sena, 's' for Dia De Sorte, 'n' for Lotomania, 'f' for Federal: ");
    scanf(" %c", &choice);

/*choice do tipo de jogo.*/
    switch (choice)
    {
        case 'q':
            Quina(validateInput);
            break;
        case 't':
            Timemania(validateInput);
            break;
        case 'l':
            Lotofacil(validateInput);
            break;
        case 'm':
            Megasena(validateInput);
            break;
        case 'd':
            Dupla_Sena(validateInput);
            break;
        case 's':
            DiaDeSorte(validateInput);
            break;
        case 'n':
            Lotomania(validateInput);
            break;
        case 'f':
            Federal();
            break;
        default:
            printf("NOTHING...\n");
            break;
    }
    return 0;
}

/*Funções Especificas para cada tipo de jogo.*/
/*QUINA*/
void Quina(int validateInput)
{
    int array_quina[5];
    int array_quina_user[5];
    int i = 0;
    int j = 0;
    int x = 0;

    while(j < 5)
    {
        printf("Enter a number (ONLY IN PERIOD [0,80]): ");
        scanf(" %d", &validateInput);
        if (validateInput > 0 && validateInput <= 80)
        {
            array_quina_user[j] = validateInput;
            j++;
        }
        else
        {
            printf("Invalid Input...\n");
            continue;
        }
    }

    printf("\n");
    printf("The sequence drawn for Quina by Caixa is: ");

    for (i = 0 ; i < 5 ; i++)
    {
        array_quina[i] = 1 + ( rand() % 79 );
        printf("%d ", array_quina[i]);
    }

    printf("\n");
    printf("\n");

    for (x = 0; x < 5; x++)
    {
        if (array_quina[x] == array_quina_user[x])
        {
            printf("You got the %d number in %d position .\n", array_quina_user[x], x+1);
        }
    }
}

/*TIMEMANIA*/
void Timemania(int validateInput)
{
    int array_time[10];
    int array_time_user[10];
    int i = 0;
    int j = 0;
    int x = 0;

    while(j < 10)
    {
        printf("Enter a number (ONLY IN PERIOD [0,80]): ");
        scanf(" %d", &validateInput);
        if (validateInput > 0 && validateInput <= 80)
        {
            array_time_user[j] = validateInput;
            j++;
        }
        else
        {
            printf("Invalid Input...\n");
            continue;
        }
    }

    printf("\n");
    printf("The sequence drawn for Timemania by Caixa is: ");

    for (i = 0 ; i < 10 ; i++)
    {
        array_time[i] = 1 + ( rand() % 79 );
        printf("%d ", array_time[i]);
    }

    printf("\n");
    printf("\n");

    for (x = 0; x < 10; x++)
    {
        if (array_time[x] == array_time_user[x])
        {
            printf("You got the %d number in %d position .\n", array_time_user[x], x+1);
        }
    }
}

/*LOTOFACIL*/
void Lotofacil(int validateInput)
{
    int array_loto[15];
    int array_loto_user[15];
    int i = 0;
    int j = 0;
    int x = 0;

    while(j < 15)
    {
        printf("Enter a number (ONLY IN PERIOD [0,25]): ");
        scanf(" %d", &validateInput);
        if (validateInput > 0 && validateInput <= 25)
        {
            array_loto_user[j] = validateInput;
            j++;
        }
        else
        {
            printf("Invalid Input...\n");
            continue;
        }
    }

    printf("\n");
    printf("The sequence drawn for Timemania by Caixa is: ");

    for (i = 0 ; i < 15 ; i++)
    {
        array_loto[i] = 1 + ( rand() % 24 );
        printf("%d ", array_loto[i]);
    }

    printf("\n");
    printf("\n");

    for (x = 0; x < 15; x++)
    {
        if (array_loto[x] == array_loto_user[x])
        {
            printf("You got the %d number in %d position .\n", array_loto_user[x], x+1);
        }
    }
}
/*MEGASENA*/
void Megasena(int validateInput)
{
    int array_mega[6];
    int array_mega_user[6];
    int i = 0;
    int j = 0;
    int x = 0;

    while(j < 6)
    {
        printf("Enter a number (ONLY IN PERIOD [0,60]): ");
        scanf(" %d", &validateInput);
        if (validateInput > 0 && validateInput <= 60)
        {
            array_mega_user[j] = validateInput;
            j++;
        }
        else
        {
            printf("Invalid Input...\n");
            continue;
        }
    }

    printf("\n");
    printf("The sequence drawn for Megasena by Caixa is: ");

    for (i = 0 ; i < 6 ; i++)
    {
        array_mega[i] = 1 + ( rand() % 59 );
        printf("%d ", array_mega[i]);
    }

    printf("\n");
    printf("\n");

    for (x = 0; x < 6; x++)
    {
        if (array_mega[x] == array_mega_user[x])
        {
            printf("You got the %d number in %d position .\n", array_mega_user[x], x+1);
        }
    }
}  
/*DUPLA-SENA*/
void Dupla_Sena(int validateInput)
{

    int array_1_dupla[6];
    int array_1_dupla_user[6];
    int i = 0;
    int j = 0;
    int x = 0;

    printf("FIRST DRAW\n");
    
    while(j < 6)
    {
        printf("Enter a number (ONLY IN PERIOD [0,50]): ");
        scanf(" %d", &validateInput);
        if (validateInput > 0 && validateInput <= 50)
        {
            array_1_dupla_user[j] = validateInput;
            j++;
        }
        else
        {
            printf("Invalid Input...\n");
            continue;
        }
    }

    printf("\n");
    printf("The first sequence drawn for Dupla-Sena by Caixa is: ");

    for (i = 0 ; i < 6 ; i++)
    {
        array_1_dupla[i] = 1 + ( rand() % 49 );
        printf("%d ", array_1_dupla[i]);
    }

    printf("\n");
    printf("\n");

    for (x = 0; x < 6; x++)
    {
        if (array_1_dupla[x] == array_1_dupla_user[x])
        {
            printf("You got the %d number in %d position .\n", array_1_dupla_user[x], x+1);
        }
    }
    
    void SegundoSorteio(int validateInput);
    SegundoSorteio(validateInput);
}     

/*SECOND DRAW DUPLA-SENA*/
void SegundoSorteio(int validateInput)
{
  
    int array_2_dupla[6];
    int array_2_dupla_user[6];
    int i = 0;
    int j = 0;
    int x = 0;
    
    printf("SECOND DRAW\n");
    
    while(j < 6)
    {
        printf("Enter a number (ONLY IN PERIOD [0,50]): ");
        scanf(" %d", &validateInput);
        if (validateInput > 0 && validateInput <= 50)
        {
            array_2_dupla_user[j] = validateInput;
            j++;
        }
        else
        {
            printf("Invalid Input...\n");
            continue;
        }
    }

    printf("\n");
    printf("The second sequence drawn for Dupla-Sena by Caixa is: ");

    for (i = 0 ; i < 6 ; i++)
    {
        array_2_dupla[i] = 1 + ( rand() % 49 );
        printf("%d ", array_2_dupla[i]);
    }

    printf("\n");
    printf("\n");

    for (x = 0; x < 6; x++)
    {
        if (array_2_dupla[x] == array_2_dupla_user[x])
        {
            printf("You got the %d number in %d position .\n", array_2_dupla_user[x], x+1);
        }
    }   
}    

/*DIA DE SORTE*/
void DiaDeSorte(int validateInput)
{
    int array_dia[7];
    int array_dia_user[7];
    int i = 0;
    int j = 0;
    int x = 0;

    while(j < 7)
    {
        printf("Enter a number (ONLY IN PERIOD [0,31]): ");
        scanf(" %d", &validateInput);
        if (validateInput > 0 && validateInput <= 31)
        {
            array_dia_user[j] = validateInput;
            j++;
        }
        else
        {
            printf("Invalid Input...\n");
            continue;
        }
    }

    printf("\n");
    printf("The sequence drawn for Dia De Sorte by Caixa is: ");

    for (i = 0 ; i < 7 ; i++)
    {
        array_dia[i] = 1 + ( rand() % 30 );
        printf("%d ", array_dia[i]);
    }

    printf("\n");
    printf("\n");

    for (x = 0; x < 7; x++)
    {
        if (array_dia[x] == array_dia_user[x])
        {
            printf("You got the %d number in %d position .\n", array_dia_user[x], x+1);
        }
    }
    void Day();
    Day();
}

/*DRAW --- DAY OF WEEK*/
void Day()
{
    char bank[7][10]={ "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    
    int i, num = 0;
    char word[10];
    
    srand(time(NULL));
    num = rand() % 6;
    
    for(i = 0; i < 10; i++)
    {
        word[i] = bank[num][i];
    }

    printf("The day of week is: %s.\n", word); 
}    
    
/*LOTOMANIA*/
void Lotomania(int validateInput)
{
    int array_mania[50];
    int array_mania_user[50];
    int i = 0;
    int j = 0;
    int x = 0;
    char escolha;
    
    printf("WARNING !!!!\n");
    printf("The following draw is too long (you will have to choose 50 numbers), do you really want to continue? (say y to yes or n to no): ");
    scanf(" %c", &escolha);
    
    if(escolha == 'y')
    {
        while(j < 50)
        {
            printf("Enter a number (ONLY IN PERIOD [0,50]): ");
            scanf(" %d", &validateInput);
            if (validateInput > 0 && validateInput <= 50)
            {
                array_mania_user[j] = validateInput;
                j++;
            }
            else
            {
                printf("Invalid Input...\n");
                continue;
            }
        }

        printf("\n");
        printf("The sequence drawn for Lotomania by Caixa is: ");

        for (i = 0 ; i < 50 ; i++)
        {
            array_mania[i] = 1 + ( rand() % 49 );
            printf("%d ", array_mania[i]);
        }

        printf("\n");
        printf("\n");

        for (x = 0; x < 50; x++)
        {
            if (array_mania[x] == array_mania_user[x])
            {
                printf("You got the %d number in %d position .\n", array_mania_user[x], x+1);
            }
        }
    }
    else if(escolha == 'n')
    {
        exit(1);
    }    
}      
    
/*FEDERAL*/
void Federal()
{
    int bilhetes[5] = {0};
    int copia_Bilhetes[5] = {0};
    int i;
    
    srand((unsigned)time(NULL));
    
    for (i = 0 ; i < 5 ; i++)
    {
        bilhetes[i] =  10000 + ( rand() % 99999 );
        printf("O bilhete %d é %d.\n", i+1, bilhetes[i]);
        copia_Bilhetes[i] = bilhetes[i];
    }

    printf("O bilhete é %d.\n", copia_Bilhetes[0]);
    printf("O bilhete é %d.\n", copia_Bilhetes[1]);
    printf("O bilhete é %d.\n", copia_Bilhetes[2]);
    printf("O bilhete é %d.\n", copia_Bilhetes[3]);
    printf("O bilhete é %d.\n", copia_Bilhetes[4]);
    
    
    
}    
    
    
    
    
    
    




