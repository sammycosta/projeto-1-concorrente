#include <stdlib.h>
#include <semaphore.h>

#include "chef.h"
#include "config.h"
#include "globals.h" //tem como fazer sem incluir??

/* Struct recebe dados do local sem comida quando ele existir */
struct dados_buffet
{
    // buffet_t *buffet_vazio;
    int buffet_vazio;
    int bacia;
};

struct dados_buffet dados_buffet = {-1, -1};

void *chef_run()
{
    // int number_of_buffets = globals_get_number_of_buffets();
    // Vai embora quando todos estudantes se serviram
    // dados_buffet = (struct dados_buffet *)(malloc(sizeof(struct dados_buffet)));

    while (TRUE)
    {

        msleep(5000);      /* Pode retirar este sleep quando implementar a solução! */
        chef_check_food(); /* Checa bacias de todos buffets */
        if (dados_buffet.bacia != -1)
        {
            /* Bacia recebeu número de bacia vazia */
            chef_put_food();
        }

        /* Garanto fim de execução. Mas está olhando a fila externa, não interna; */
        int all_students_entered;
        int number_students = globals_get_students();
        all_students_entered = number_students > 0 ? FALSE : TRUE;

        if (all_students_entered == TRUE)
            break;
    }
    printf("chef saiu\n");
    // free(dados_buffet);
    pthread_exit(NULL);
}

/* Adiciona 40 de comida na bacia vazia utilizando a struct */
void chef_put_food()
{
    buffet_t *buffets = globals_get_buffets();
    buffets[dados_buffet.buffet_vazio]._meal[dados_buffet.bacia] = 40; // acho que não precisa mutex!
    for (int i = 0; i < 40; i++)
    {
        sem_post(&buffets[dados_buffet.buffet_vazio].sem_meals[dados_buffet.bacia]);
    }
}

/* Checa todas bacias de todos os buffets e altera struct caso encontrar uma vazia.
Caso não exista bacia vazia, dados->bacia recebe -1 */
void chef_check_food()
{
    buffet_t *buffets = globals_get_buffets();
    int number_of_buffets = globals_get_number_of_buffets();
    for (int i = 0; i < number_of_buffets; i++)
    {
        for (int bacia = 0; bacia < 5; bacia++)
        {
            if (buffets[i]._meal[bacia] == 0)
            {
                dados_buffet.buffet_vazio = i;
                dados_buffet.bacia = bacia;
                return;
            }
            else
            {
                dados_buffet.bacia = -1;
            }
        }
    }
    return;
}

/* --------------------------------------------------------- */
/* ATENÇÃO: Não será necessário modificar as funções abaixo! */
/* --------------------------------------------------------- */

void chef_init(chef_t *self)
{
    pthread_create(&self->thread, NULL, chef_run, NULL);
}

void chef_finalize(chef_t *self)
{
    pthread_join(self->thread, NULL);
    free(self);
}