#include <stdlib.h>
#include <semaphore.h>

#include "chef.h"
#include "config.h"
#include "globals.h"

struct dados_buffet
{
    int buffet_vazio;
    int bacia;
};

struct dados_buffet dados_buffet = {-1, -1}; // Struct recebe dados do local sem comida quando ele existir

void *chef_run()
{
    // Vai embora quando todos estudantes se serviram
    int number_students = globals_get_students(); // TOTAL DE ESTUDANTES NO INICIO

    while (TRUE)
    {

        msleep(2000);
        chef_check_food(); /* Checa bacias de todos buffets */
        if (dados_buffet.bacia != -1)
        {
            chef_put_food(); // Bacia recebeu informações sobre bacia vazia
        }

        /* Garanto fim de execução. Baseado no número de estudantes que sairam da função de serve */
        int students_served = globals_get_students_served();
        if (students_served == number_students)
            break;
    }
    pthread_exit(NULL);
}

/* Adiciona 40 de comida na bacia vazia utilizando a struct */
void chef_put_food()
{
    buffet_t *buffets = globals_get_buffets();
    buffets[dados_buffet.buffet_vazio]._meal[dados_buffet.bacia] = 40; // Realiza a reposição de comida
    for (int i = 0; i < 40; i++)                                       // (Só há 1 chef, então não precisa mutex)
    {
        // Pos no semáforo 40 vezes (número total de porções disponíveis)
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
                dados_buffet.buffet_vazio = i; // Há bacia vazia. Salvar onde ela está
                dados_buffet.bacia = bacia;
                return;
            }
            else
            {
                dados_buffet.bacia = -1; // Não há bacia vazia
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