#include <stdlib.h>
#include "globals.h"
#include "config.h"

queue_t *students_queue = NULL;
table_t *table = NULL;
buffet_t *buffets_ref = NULL;

int number_of_buffets = 0;
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

void globals_set_number_of_buffets(int number)
{
    number_of_buffets = number;
}

int globals_get_number_of_buffets()
{
    return students_number;
}

/**
 * @brief Finaliza todas as variáveis globais que ainda não foram liberadas.
 *  Se criar alguma variável global que faça uso de mallocs, lembre-se sempre de usar o free dentro
 * dessa função.
 */
void globals_finalize()
{

    /* Destruir mutexes aqui por enquanto */
    for (int i = 0; i < number_of_buffets; i++)
    {
        for (int j = 0; j < 5; j++)
        { // destruindo mutex por bacia
            pthread_mutex_destroy(&buffets_ref[i].mutex_meals[j]);
        }
    }
    /* */
    free(table);
}