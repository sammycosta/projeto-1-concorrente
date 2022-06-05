#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "student.h"

    struct queue_item
    {
        student_t *_student; /* O ponteiro que aponta para um estudante*/
        struct queue_item* _next; /* Ponteiro que aponta para o próximo estudante da fila*/
    };

    typedef struct queue
    {
        int _length; /* Tamanho atual da fila */
        struct queue_item *_first; /* Primeiro da Fila */
        struct queue_item *_last;  /* Último da fila */
    } queue_t;

    /**
     * @brief Inicia a fila e retorna um ponteiro para ela.
     * 
     * @return queue_t* 
     */
    extern queue_t *queue_init();
    
    /**
     * @brief Finaliza uma fila, liberando todos os espaços em memória que estejam (ainda) alocados. 
     * 
     * @param self 
     */
    extern void queue_finalize(queue_t *self);
    
    /**
     * @brief Insere um elemento do tipo student_t na fila.
     * 
     * @param self 
     * @param student 
     * @return queue_t* 
     */
    extern queue_t *queue_insert(queue_t *self, student_t *student);
    
    /**
     * @brief Retorna um estudante do início da fila.
     * 
     * @param self 
     * @return student_t* 
     */
    extern student_t *queue_remove(queue_t *self);

#endif /*__CONFIG_H__*/