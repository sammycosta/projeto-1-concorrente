#include <stdlib.h>

#include "chef.h"
#include "config.h"
#include "globals.h" //tem como fazer sem incluir??

/* Struct recebe dados do local sem comida quando ele existir */
struct dados_buffet
{
    buffet_t buffet_vazio;
    int bacia;
}; // precisa de uma struct? É melhor fazer sem?

void *chef_run()
{
    // Vai embora quando todos estudantes se serviram
    struct dados_buffet *dados_buffet = (struct dados_buffet *)(malloc(sizeof(struct dados_buffet)));
    while (TRUE)
    {
        msleep(5000);                  /* Pode retirar este sleep quando implementar a solução! */
        chef_check_food(dados_buffet); /* Checa bacias de todos buffets */
        if (dados_buffet->bacia != -1)
        {
            /* Bacia recebeu número de bacia vazia */
            chef_put_food(dados_buffet);
        }
    }
    free(dados_buffet);
    pthread_exit(NULL);
}

/* Adiciona 40 de comida na bacia vazia utilizando a struct */
void chef_put_food(struct dados_buffet *dados)
{
    dados->buffet_vazio._meal[dados->bacia] = 40; // acho que não precisa mutex!
}

/* Checa todas bacias de todos os buffets e altera struct caso encontrar uma vazia.
Caso não exista bacia vazia, dados->bacia recebe -1 */
void chef_check_food(struct dados_buffet *dados)
{
    buffet_t *buffets = globals_get_buffets();
    int number_of_buffets = globals_get_number_of_buffets();

    for (int i = 0; i < number_of_buffets; i++)
    {
        for (int bacia = 0; i < 5; i++)
        {
            if (buffets[i]._meal[bacia] == 0)
            {
                dados->buffet_vazio = buffets[i];
                dados->bacia = bacia;
                return;
            }
            else
            {
                dados->bacia = -1;
            }
        }
    }
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