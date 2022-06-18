#include <stdlib.h>
#include "buffet.h"
#include "config.h"
#include "globals.h"
#include <semaphore.h>

void *buffet_run(void *arg)
{
    int all_students_entered = FALSE;
    buffet_t *self = (buffet_t *)arg;

    /*  O buffet funciona enquanto houver alunos na fila externa. */
    while (all_students_entered == FALSE)
    {
        /* Cada buffet possui: Arroz, Feijão, Acompanhamento, Proteína e Salada */
        /* Máximo de porções por bacia (40 unidades). */
        _log_buffet(self);

        // mesma coisa que o worker gate faz. Talvez alterar  all_students_entered para global
        // int number_students = globals_get_queue()->_length;

        int number_students = globals_get_students();
        all_students_entered = number_students > 0 ? FALSE : TRUE;

        msleep(5000); /* Pode retirar este sleep quando implementar a solução! */
    }

    pthread_exit(NULL);
}

void buffet_init(buffet_t *self, int number_of_buffets)
{
    int i = 0, j = 0;
    globals_set_number_of_buffets(number_of_buffets);

    for (i = 0; i < number_of_buffets; i++)
    {
        /*A fila possui um ID*/
        self[i]._id = i;

        /* Inicia com 40 unidades de comida em cada bacia */
        for (j = 0; j < 5; j++)
        {
            self[i]._meal[j] = 40;
            pthread_mutex_init(&self[i].mutex_meals[j], NULL);
            sem_init(&(self[i].sem_meals[j]), 0, 40);
            sem_init(&self[i].controle_fila_dir[j], 0, 1);
            sem_init(&self[i].controle_fila_esq[j], 0, 1);
        }
        /*for (a = 0; a < 4; a++)
        {
            sem_init(&self[i].controle_fila_dir[a], 0, 1);
            sem_init(&self[i].controle_fila_esq[a], 0, 1);
        }*/

        for (j = 0; j < 5; j++)
        {
            /* A fila esquerda do buffet possui cinco posições. */
            self[i].queue_left[j] = 0;
            /* A fila esquerda do buffet possui cinco posições. */
            self[i].queue_right[j] = 0;
        }

        pthread_create(&self[i].thread, NULL, buffet_run, &self[i]);
    }
}

int buffet_queue_insert(buffet_t *self, student_t *student)
{
    /* Se o estudante vai para a fila esquerda */
    if (student->left_or_right == 'L')
    {
        /* Verifica se a primeira posição está vaga */ 
        //if (!self[student->_id_buffet].queue_left[0])
        //{
            sem_wait(&self[student->_id_buffet].controle_fila_esq[0]);
            self[student->_id_buffet].queue_left[0] = student->_id;
            student->_buffet_position = 0;
            pthread_mutex_unlock(&student->mutex);                  // libero student_serve
            pthread_mutex_t *mutex_gate = globals_get_mutex_gate(); // pego end do sai_fila
            pthread_mutex_unlock(mutex_gate);
            return TRUE;
        //}
        printf("falhou entrar \n");
        return FALSE;
    }
    else
    { /* Se o estudante vai para a fila direita */
        //if (!self[student->_id_buffet].queue_right[0])
        //{
            /* Verifica se a primeira posição está vaga */
            sem_wait(&self[student->_id_buffet].controle_fila_dir[0]);
            self[student->_id_buffet].queue_right[0] = student->_id;
            student->_buffet_position = 0;
            pthread_mutex_unlock(&student->mutex);                  // libero student_serve
            pthread_mutex_t *mutex_gate = globals_get_mutex_gate(); // pego end do sai_fila
            pthread_mutex_unlock(mutex_gate);

            return TRUE;
        //}
        printf("falhou entrar \n");
        return FALSE;
    }
}

void buffet_next_step(buffet_t *self, student_t *student)
{
    /* Se estudante ainda precisa se servir de mais alguma coisa... */
    if (student->_buffet_position + 1 < 5)
    { /* Está na fila esquerda? */
        if (student->left_or_right == 'L')
        { /* Caminha para a posição seguinte da fila do buffet.*/
            int position = student->_buffet_position;

            sem_wait(&(self[student->_id_buffet].controle_fila_esq[position + 1]));
            self[student->_id_buffet].queue_left[position] = 0;
            self[student->_id_buffet].queue_left[position + 1] = student->_id;
            student->_buffet_position = student->_buffet_position + 1;

            printf("-> dentro do semaforo: estudante %d andou na fila\n", student->_id);
            sem_post(&(self[student->_id_buffet].controle_fila_esq[position]));
        }
        else /* Está na fila direita? */
        {    /* Caminha para a posição seguinte da fila do buffet.*/
            int position = student->_buffet_position;

            sem_wait(&(self[student->_id_buffet].controle_fila_dir[position + 1]));
            self[student->_id_buffet].queue_right[position] = 0;
            self[student->_id_buffet].queue_right[position + 1] = student->_id;
            student->_buffet_position = student->_buffet_position + 1;

            printf("-> dentro do semaforo: estudante %d andou na fila\n", student->_id);
            sem_post(&(self[student->_id_buffet].controle_fila_dir[position]));
        }
    }
    else
    {
        // lembrar que tenho que zerar a pos quando ele vai ir embora tmb?
        if (student->left_or_right == 'L')
        {
            self[student->_id_buffet].queue_left[4] = 0;
            sem_post(&(self[student->_id_buffet].controle_fila_esq[student->_buffet_position]));
        }
        else
        {
            self[student->_id_buffet].queue_right[4] = 0;
            sem_post(&(self[student->_id_buffet].controle_fila_dir[student->_buffet_position]));
        }
        student->_buffet_position = student->_buffet_position + 1; // assim ele sai?
    }
}

/* --------------------------------------------------------- */
/* ATENÇÃO: Não será necessário modificar as funções abaixo! */
/* --------------------------------------------------------- */

void buffet_finalize(buffet_t *self, int number_of_buffets)
{
    /* Espera as threads se encerrarem...*/
    for (int i = 0; i < number_of_buffets; i++)
    {
        pthread_join(self[i].thread, NULL);
    }

    /*Libera a memória.*/
    free(self);
}

void _log_buffet(buffet_t *self)
{
    /* Prints do buffet */
    int *ids_left = self->queue_left;
    int *ids_right = self->queue_right;

    printf("\n\n\u250F\u2501 Queue left: [ %d %d %d %d %d ]\n", ids_left[0], ids_left[1], ids_left[2], ids_left[3], ids_left[4]);
    fflush(stdout);
    printf("\u2523\u2501 BUFFET %d = [RICE: %d/40 BEANS:%d/40 PLUS:%d/40 PROTEIN:%d/40 SALAD:%d/40]\n",
           self->_id, self->_meal[0], self->_meal[1], self->_meal[2], self->_meal[3], self->_meal[4]);
    fflush(stdout);
    printf("\u2517\u2501 Queue right: [ %d %d %d %d %d ]\n", ids_right[0], ids_right[1], ids_right[2], ids_right[3], ids_right[4]);
    fflush(stdout);
}