#include <stdlib.h>
#include "globals.h"
#include "config.h" // ADICIONEI PARA PEGAR NUMBER_OF_BUFFETS

queue_t *students_queue = NULL;
table_t *table = NULL;
buffet_t *buffets_ref = NULL;
pthread_mutex_t *queues_left = NULL;
pthread_mutex_t *queues_right = NULL;

int students_number = 0;

void globals_set_queue(queue_t *queue)
{
    students_queue = queue;
}

queue_t *globals_get_queue()
{
    return students_queue;
}

void globals_set_table(table_t *t)
{
    table = t;
}

table_t *globals_get_table()
{
    return table;
}

void globals_set_students(int number)
{
    students_number = number;
}

int globals_get_students()
{
    return students_number;
}

void globals_set_buffets(buffet_t *buffets)
{
    buffets_ref = buffets;
}

buffet_t *globals_get_buffets()
{
    return buffets_ref;
}

/* Meus códigos abaixo */
pthread_mutex_t *globals_get_queues_left()
{
    return queues_left;
}

void globals_set_queues_left(pthread_mutex_t *new_queue)
{
    queues_left = new_queue;
}

pthread_mutex_t *globals_get_queues_right()
{
    return queues_right;
}

void globals_set_queues_right(pthread_mutex_t *new_queue)
{
    queues_right = new_queue;
}

/**
 * @brief Finaliza todas as variáveis globais que ainda não foram liberadas.
 *  Se criar alguma variável global que faça uso de mallocs, lembre-se sempre de usar o free dentro
 * dessa função.
 */
void globals_finalize()
{
    free(table);

    /* QUE OUTRO LUGAR EU PODERIA DAR DESTROY, SE BUFFETS_FINALIZE NÃO PODE? */
    for (int i = 0; i < config.buffets; i++)
    {
        pthread_mutex_destroy(&queues_left[i]);
        pthread_mutex_destroy(&queues_right[i]);
    }

    free(queues_left);
    free(queues_right);
}