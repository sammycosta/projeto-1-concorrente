#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>

#include "student.h"
#include "config.h"
#include "worker_gate.h"
#include "globals.h"
#include "table.h"

void *student_run(void *arg)
{
    student_t *self = (student_t *)arg;
    table_t *tables = globals_get_table();

    /* Inicia mutex pessoal do estudante */
    pthread_mutex_init(&self->mutex, NULL);
    pthread_mutex_lock(&self->mutex);

    /* Estudante entra na fila única externa  */
    pthread_mutex_t *mutex_queue_insert = globals_get_mutex_queue_insert();
    pthread_mutex_lock(mutex_queue_insert);
    queue_t *fila_de_fora = globals_get_queue();
    queue_insert(fila_de_fora, self);
    pthread_mutex_unlock(mutex_queue_insert);

    /* Estudante espera ser removido da fila externa para ser inserido no buffet */
    pthread_mutex_lock(&self->mutex);
    worker_gate_insert_queue_buffet(self);

    /* Estudante espera ser inserido adequadamente no buffet antes de começar a se servir */
    pthread_mutex_lock(&self->mutex);
    student_serve(self);

    /* Estudante terminou de se servir. Altero o contador global protegido por mutex */
    pthread_mutex_t *mutex_served = globals_get_mutex_served();
    pthread_mutex_lock(mutex_served);
    int students_served = globals_get_students_served() + 1;
    globals_set_students_served(students_served);
    pthread_mutex_unlock(mutex_served);

    /* Estudante livre para se sentar, comer e ir embora */
    student_seat(self, tables);
    msleep(2000); // tempo de comer
    student_leave(self, tables);

    /* Estudante foi embora. Altero o contador global protegido por mutex */
    pthread_mutex_t *mutex_gone = globals_get_mutex_gone();
    pthread_mutex_lock(mutex_gone);
    int students_gone = globals_get_students_gone() + 1;
    globals_set_students_gone(students_gone);
    pthread_mutex_unlock(mutex_gone);

    pthread_exit(NULL);
};

void student_seat(student_t *self, table_t *table)
{
    int i = 0;
    int number_of_tables = globals_get_number_of_tables();
    pthread_mutex_t *pegar_cadeira = globals_get_mutex_seats();
    while (i < number_of_tables)
    {                                          // fica no loop olhando as mesas até achar uma com lugar vago
        pthread_mutex_lock(&pegar_cadeira[i]); // proteção dos valores
        if (table[i]._empty_seats > 0)
        {
            table[i]._empty_seats--;
            pthread_mutex_unlock(&pegar_cadeira[i]);
            self->_id_buffet = table[i]._id; // salvando a mesa onde antes estava o buffet
            return;                          // (não tem variável específica para mesa, não usamos mais _id_buffet)
        }
        else
        {
            pthread_mutex_unlock(&pegar_cadeira[i]);
            i = (i + 1) % number_of_tables; //  quando as mesas acabarem, recomeça da primeira
        }
    }
}

void student_serve(student_t *self)
{
    buffet_t *buffet = globals_get_buffets();
    int id_buffet = self->_id_buffet;

    while (self->_buffet_position < 5)
    {
        if (self->_wishes[self->_buffet_position] == 1)
        {
            // semáforo por bacia garante que o estudante só começa a se servir se houver comida
            // (inicializado com a quantidade total de porções)
            sem_wait(&buffet[id_buffet].sem_meals[self->_buffet_position]);

            // lock no mutex da bacia (um mutex para cada bacia, de cada buffet)
            // garante exclusão mútua no decremento
            pthread_mutex_lock(&buffet[id_buffet].mutex_meals[self->_buffet_position]);

            buffet[id_buffet]._meal[self->_buffet_position] -= 1;

            // libera o mutex da bacia em que o student está pegando comida
            pthread_mutex_unlock(&buffet[id_buffet].mutex_meals[self->_buffet_position]);
        }
        msleep(1000); // tempo de se servir
        buffet_next_step(buffet, self);
    }
}

void student_leave(student_t *self, table_t *table)
{

    /* Libera a cadeira em que estava sentado */
    pthread_mutex_t *pegar_cadeira = globals_get_mutex_seats();
    pthread_mutex_lock(&pegar_cadeira[self->_id_buffet]);
    table[self->_id_buffet]._empty_seats++;
    pthread_mutex_unlock(&pegar_cadeira[self->_id_buffet]);

    /* Destrói o mutex pessoal */
    pthread_mutex_destroy(&self->mutex);
}

/* --------------------------------------------------------- */
/* ATENÇÃO: Não será necessário modificar as funções abaixo! */
/* --------------------------------------------------------- */

student_t *student_init()
{
    student_t *student = malloc(sizeof(student_t));
    student->_id = rand() % 1000;
    student->_buffet_position = -1;
    int none = TRUE;
    for (int j = 0; j <= 4; j++)
    {
        student->_wishes[j] = _student_choice();
        if (student->_wishes[j] == 1)
            none = FALSE;
    }

    if (none == FALSE)
    {
        /* O estudante só deseja proteína */
        student->_wishes[3] = 1;
    }

    return student;
};

void student_finalize(student_t *self)
{
    free(self);
};

pthread_t students_come_to_lunch(int number_students)
{
    pthread_t lets_go;
    pthread_create(&lets_go, NULL, _all_they_come, &number_students);
    return lets_go;
}

/**
 * @brief Função (privada) que inicializa as threads dos alunos.
 *
 * @param arg
 * @return void*
 */
void *_all_they_come(void *arg)
{
    int number_students = *((int *)arg);

    student_t *students[number_students];

    for (int i = 0; i < number_students; i++)
    {
        students[i] = student_init(); /* Estudante é iniciado, recebe um ID e escolhe o que vai comer*/
    }

    for (int i = 0; i < number_students; i++)
    {
        pthread_create(&students[i]->thread, NULL, student_run, students[i]); /*  Cria as threads  */
    }

    for (int i = 0; i < number_students; i++)
    {
        pthread_join(students[i]->thread, NULL); /*  Aguarda o término das threads   */
    }

    for (int i = 0; i < number_students; i++)
    {
        student_finalize(students[i]); /*  Libera a memória de cada estudante  */
    }

    pthread_exit(NULL);
}

/**
 * @brief Função que retorna as escolhas dos alunos, aleatoriamente (50% para cada opção)
 *        retornando 1 (escolhido) 0 (não escolhido). É possível que um aluno não goste de nenhuma opção
 *         de comida. Nesse caso, considere que ele ainda passa pela fila, como todos aqueles que vão comer.
 * @return int
 */
int _student_choice()
{
    float prob = (float)rand() / RAND_MAX;
    return prob > 0.51 ? 1 : 0;
}