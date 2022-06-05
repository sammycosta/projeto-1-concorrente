#ifndef __STUDENT_H__
#define __STUDENT_H__

#include <pthread.h>
#include "table.h"

typedef struct student
{
    int _id;                                /* Id do aluno */
    int _wishes[5];                         /* salada, arroz, feijão, acompanhamento, proteína */
    int _buffet_position;                   /* Posição na fila do buffet*/
    int _id_buffet;                         /* Qual buffet o estudante está alocado?*/
    char left_or_right;                     /* Fila da esquerda(L) ou da direita(R)*/
    pthread_t thread;                       /* A thread */
} student_t;

/**
* @brief Inicializa um estudente.
* 
* @return student_t* 
*/
student_t *student_init();

/**
* @brief Libera o espaço em memória de um estudante
* 
* @param self 
*/
extern void student_finalize (student_t *self);

/**
* @brief Thread com a lógica para cada aluno.
* 
* @param arg 
* @return void* 
*/
extern void *student_run(void *arg);

/**
* @brief Função que chama a criação das threads dos alunos. 
* 
* @param number_students 
* @return pthread_t 
*/
extern pthread_t students_come_to_lunch(int number_students);

/**
* @brief Função que faz com que o estudante sente em alguma mesa.
* 
* @param self 
* @param table 
*/
extern void student_seat (student_t *self, table_t *table);

/**
* @brief Função que faz com que o estudante se sirva em algum buffet
* 
* @param self 
*/
extern void student_serve (student_t *self);


/**
* @brief Função que faz com que o estudante saia do restaurante
* 
* @param self 
*/
extern void student_leave (student_t *self, table_t *table);

/**
 * @brief Referências para funções privadas ao arquivo.
 * 
 * @param self 
 */
void* _all_they_come(void *arg);
int _student_choice();

#endif /*__STUDENT_H__*/