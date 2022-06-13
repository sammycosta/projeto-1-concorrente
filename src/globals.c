#include <stdlib.h>
#include "globals.h"
#include "config.h"
#include "semaphore.h"

queue_t *students_queue = NULL;
table_t *table = NULL;
buffet_t *buffets_ref = NULL;
pthread_mutex_t *pegar_cadeira;

int number_of_buffets = 0;
int students_number = 0;
int number_of_tables = 0;
int seats_per_table = 0;

extern pthread_mutex_t *pegar_cadeira; // variavel global, melhor fazer função?

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
    return number_of_buffets;
}

void globals_set_number_of_tables(int number)
{
    number_of_tables = number;
}

int globals_get_number_of_tables()
{
    return number_of_tables;
}

void globals_set_seats_per_table(int number)
{
    seats_per_table = number;
}

int globals_get_seats_per_table()
{
    return seats_per_table;
}

void init_mutexes() // inicia o mutex das mesas
{
    pthread_mutex_t *pegar_cadeira = malloc(sizeof(pegar_cadeira) * number_of_tables);
    for (int i = 0; i < number_of_tables; i++)
    {
        pthread_mutex_init(&(pegar_cadeira[i]), NULL);
    }

    globals_set_mutex_seats(pegar_cadeira);
}

void globals_set_mutex_seats(pthread_mutex_t *array)
{
    pegar_cadeira = array;
}

pthread_mutex_t *globals_get_mutex_seats()
{
    return pegar_cadeira;
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

    for (int i = 0; i < number_of_tables; i++)
    {
        pthread_mutex_destroy(&(pegar_cadeira[i]));
    }
    /* */
    free(table);
}