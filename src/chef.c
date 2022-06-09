#include <stdlib.h>

#include "chef.h"
#include "config.h"
#include "globals.h" //tem como fazer sem incluir??

struct dados_buffet
{
    buffet_t buffet_vazio;
    int bacia;
}; //precisa de uma struct? É melhor fazer sem?


void *chef_run()
{
    /* Insira sua lógica aqui */
    /* checa comida de todas bacias de todos buffets,
    coloca comida,
    vai embora quando todos estudantes se serviram*/
    struct dados_buffet *dados_buffet = (struct dados_buffet*)(malloc(sizeof(struct dados_buffet)));
    while (TRUE)
    {
        msleep(5000); /* Pode retirar este sleep quando implementar a solução! */
        chef_check_food(dados_buffet);
        if (dados_buffet->bacia != -1) {
            
            chef_put_food(dados_buffet);
        }        
    }
    free(dados_buffet);
    pthread_exit(NULL);
}

void chef_put_food(struct dados_buffet *dados)
{
    /* Insira sua lógica aqui */
    dados->buffet_vazio._meal[dados->bacia] = 40; //acho que não precisa mutex!

}
void chef_check_food(struct dados_buffet *dados)
{
    /* Insira sua lógica aqui */
    buffet_t* buffets = globals_get_buffets();
    for (int i = 0; i < config.buffets; i++) {
        for (int bacia  = 0; i < 5; i++) {
            if (buffets[i]._meal[bacia] == 0) {
                dados->buffet_vazio = buffets[i];
                dados->bacia = bacia;
                return;
            } else {
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