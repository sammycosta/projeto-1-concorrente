#ifndef __WORKER_GATE_H__
#define __WORKER_GATE_H__

#include <pthread.h>
#include "student.h"

typedef struct worker_gate
{
    
    pthread_t thread; // A thread do funcionário que fica na catraca.

} worker_gate_t;


/**
 * @brief Inicia o funcionário que fica na catraca.
 * 
 * @param self 
 */
extern void worker_gate_init(worker_gate_t *self);

/**
 * @brief Finaliza a thread do funcionário que fica na catraca.
 * 
 * @param self 
 */
extern void worker_gate_finalize(worker_gate_t *self);

/**
 * @brief Thread do funcionário que fica na catraca.
 * 
 * @return void* 
 */
extern void* worker_gate_run();

/**
 * @brief Chegou a vez do próximo! Função que faz com que o funcionário retire um estudante da fila.
 * 
 */
extern void worker_gate_remove_queue_student();

/**
 * @brief Função que faz com que o funcionário direcione o estudante para o buffet. 
 * 
 */
extern void worker_gate_insert_queue_buffet(student_t *student);

#endif