#include <stdlib.h>
#include <time.h>

#include "queue.h"
#include "config.h"


/* --------------------------------------------------------- */
/* ATENÇÃO: Não será necessário modificar as funções abaixo! */
/* --------------------------------------------------------- */

queue_t *queue_init()
{
    queue_t *q = malloc(sizeof(queue_t));
    q->_first = NULL;
    q->_last = NULL;
    q->_length = 0;
    return q;
}

void queue_finalize(queue_t *self)
{
    struct queue_item *item = NULL;

    /* Enquanto houverem elementos na fila, libera item a item os elementos da memória */
    for (int i = 0; i < self->_length; i = i + 1)
    {
        item = self->_first;
        self->_first = self->_first->_next;
        free(item->_student);
        free(item);
    }

    /*Finalmente, destroi a fila.*/
    free(self);
}

queue_t *queue_insert(queue_t *self, student_t *student)
{

    /* Se a fila não está inicializada */
    if (self == NULL)
    {
        self = queue_init(self);
    }

    /* Cria o elemento da fila em memória*/
    struct queue_item *item = (struct queue_item *)malloc(sizeof(struct queue_item));
    item->_student = student;
    item->_next = NULL;

    /* Tamanho = Tamanho + 1 */
    self->_length = self->_length + 1;

    
    if (self->_length == 1)
    {
        /* Se a fila está vazia, então ... */
        self->_first = item;
        self->_last = item;
    }
    else        
    {
        /* Se a fila está com elementos, então coloca na última posição da fila */
        self->_last->_next = item;
        self->_last = item;
    }

    return self;
}

student_t *queue_remove(queue_t *self)
{
    if (self->_length > 0)
    {
        /*Se a fila possui elementos, retira o primeiro elemento em ordem FIFO (First In, First Out)*/
        
        student_t *student = NULL;
        struct queue_item *item = NULL;

        student = self->_first->_student;
        item = self->_first;

        self->_first = self->_first->_next;
        self->_length = self->_length - 1;

        free(item); 
        return student;
    }

    return NULL;
}