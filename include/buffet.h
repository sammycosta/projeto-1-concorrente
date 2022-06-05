#ifndef __buffet_H__
#define __buffet_H__

#include <pthread.h>

#include "queue.h"


typedef struct buffet
{
    int _id;
    int _meal[5];

    int queue_left[5];
    int queue_right[5];
    
    pthread_t thread; /* Thread do buffet   */
} buffet_t;

/**
 * @brief Thread do buffet.
 * 
 * @return void* 
 */
extern void* buffet_run();

/**
 * @brief Inicia o buffet
 * 
 */
extern void buffet_init(buffet_t *self, int number_of_buffets);

/**
 * @brief Encerra o buffet
 * 
 */
extern void buffet_finalize(buffet_t *self, int number_of_buffets);

/**
 * @brief Vai para a próxima posição da fila do buffet 
 * 
 * @param self 
 * @param student 
 */
extern void buffet_next_step(buffet_t *self, student_t *student);

/**
 * @brief Retorna TRUE quando inseriu um estudante com sucesso no fim da fila do buffet. 
 *        Retorna FALSE, caso contrário.
 * 
 */
extern int buffet_queue_insert(buffet_t *self, student_t *student);

/**
 * @brief Referências para funções privadas ao arquivo.
 * 
 * @param self 
 */
void _log_buffet(buffet_t *self);

#endif