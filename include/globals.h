#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "queue.h"
#include "table.h"
#include "buffet.h"

/**
 * @brief Inicia uma fila (de modo global)
 *
 * @param queue
 */
extern void globals_set_queue(queue_t *queue);

/**
 * @brief Retorna uma fila (de modo global)
 *
 * @return queue_t*
 */
extern queue_t *globals_get_queue();

/**
 * @brief Insere o número de alunos (de modo global)
 *
 */
extern void globals_set_students(int number);

/**
 * @brief Retorna o número de alunos (de modo global)
 *
 * @return int
 */
extern int globals_get_students();

/**
 * @brief Inicia um array de mesas (de modo global).
 *
 * @param t
 */
extern void globals_set_table(table_t *t);

/**
 * @brief Retorna um array de mesas (de modo global)
 *
 * @return table_t*
 */
extern table_t *globals_get_table();

/**
 * @brief Finaliza todas as variáveis globais.
 *
 */
extern void globals_finalize();

/**
 * @brief Inicia um array de buffets (de modo global)
 *
 */
extern void globals_set_buffets(buffet_t *buffets_ref);

/**
 * @brief Retorna um array de buffets (de modo global)
 *
 * @return buffet_t*
 */
extern buffet_t *globals_get_buffets();

/**
 * @brief Insere o número de buffets (de modo global)
 *
 */
extern void globals_set_number_of_buffets(int number);

/**
 * @brief Retorna o número de buffets (de modo global)
 *
 * @return int
 */
extern int globals_get_number_of_buffets();

/**
 * @brief Insere o número de tables (de modo global)
 *
 */
extern void globals_set_number_of_tables(int number);

/**
 * @brief Retorna o número de tables (de modo global)
 *
 * @return int
 */
extern int globals_get_number_of_tables();

/**
 * @brief Insere o número de seats por table (de modo global)
 *
 */
extern void globals_set_seats_per_table(int number);

/**
 * @brief Inicializa os mutexes necessários (de modo global)
 *
 */
extern void init_mutexes();

/**
 * @brief Retorna o array de mutexes pegar_cadeira (de modo global)
 *
 * @return pthread_mutex_t*
 */
extern pthread_mutex_t *globals_get_mutex_seats();

/**
 * @brief Retorna o endereço do mutex da catraca
 *
 * @return pthread_mutex_t*
 */
extern pthread_mutex_t *globals_get_mutex_gate();

/**
 * @brief Retorna o número de estudantes que já se serviram no buffet
 *
 * @return int
 */
extern int globals_get_students_served();

/**
 * @brief Insere o número de estudantes que já se serviram no buffet
 *
 */
extern void globals_set_students_served(int number);

/**
 * @brief Retorna o endereço do mutex que protege incremento de students_served
 *
 * @return pthread_mutex_t*
 */
extern pthread_mutex_t *globals_get_mutex_served();

/**
 * @brief Retorna o número de estudantes que já sairam do RU
 *
 * @return int
 */
extern int globals_get_students_gone();

/**
 * @brief Insere o número de estudantes que já saíram do RU
 *
 */
extern void globals_set_students_gone(int number);

/**
 * @brief Retorna o endereço do mutex que protege incremento de students_gone
 *
 * @return pthread_mutex_t*
 */
extern pthread_mutex_t *globals_get_mutex_gone();

/**
 * @brief Retorna o endereço do mutex que protege o estudante entrando na fila externa
 *
 * @return pthread_mutex_t*
 */
extern pthread_mutex_t *globals_get_mutex_queue_insert();
#endif