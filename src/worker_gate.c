#include <stdlib.h>

#include "worker_gate.h"
#include "globals.h"
#include "config.h"
#include "buffet.h"

pthread_mutex_t catraca, sai_fila;

/* Modifica all_students_entered para TRUE quando não houver estudantes na fila externa */
void worker_gate_look_queue(int *all_students_entered)
{
    int number_students = globals_get_queue()->_length;
    *all_students_entered = number_students > 0 ? FALSE : TRUE;
}

/* Atual: pop na queue de estudantes de fora. Não faz nada com ele, só remove; */
void worker_gate_remove_student()
{
    queue_t *fila = globals_get_queue();
    queue_remove(fila);
    // student_t *estudante_saindo = queue_remove(fila);
}

/* Checa todos os buffets, encontra primeiro vazio;
Destrava mutex "catraca" e retorna o lado da fila vazia (LEFT/RIGHT)
Ou retorna N caso não houver local vazio nas filas dos buffets. */
char worker_gate_look_buffet()
{
    buffet_t *buffets = globals_get_buffets();
    for (int i = 0; i < config.buffets; i++)
    {
        if (!buffets[i].queue_left[0])
        { // precisa mutex?
            // preenche estudante e libera sua passagem
            student_t *primeiro_estudante = globals_get_queue()->_first->_student;
            primeiro_estudante->_id_buffet = buffets[i]._id;
            primeiro_estudante->left_or_right = 'L';
            pthread_mutex_unlock(&catraca);
            return 'L';
        }
        else if (!buffets[i].queue_right[0])
        {
            student_t *primeiro_estudante = globals_get_queue()->_first->_student;
            primeiro_estudante->_id_buffet = buffets[i]._id;
            primeiro_estudante->left_or_right = 'R';
            pthread_mutex_unlock(&catraca);
            return 'R';
        }
    }
    return 'N';
}

void *worker_gate_run(void *arg)
{
    int all_students_entered;
    int number_students;
    char fila_livre = 'A';

    number_students = *((int *)arg);
    all_students_entered = number_students > 0 ? FALSE : TRUE;

    while (all_students_entered == FALSE)
    {
        worker_gate_look_queue(&all_students_entered); // Decide se finaliza thread.

        fila_livre = worker_gate_look_buffet(); // unlock catraca ou não

        if (fila_livre == 'L' || fila_livre == 'R')
        {
            // look buffet executou, garantindo um buffet livre
            pthread_mutex_lock(&sai_fila); // Só sai daqui se estudante rodou função insert
            worker_gate_remove_student();
        }
        msleep(5000); /* Pode retirar este sleep quando implementar a solução! */
    }

    pthread_exit(NULL);
}

void worker_gate_init(worker_gate_t *self)
{
    int number_students = globals_get_students();
    pthread_create(&self->thread, NULL, worker_gate_run, &number_students);
    pthread_mutex_init(&catraca, NULL);
    pthread_mutex_init(&sai_fila, NULL);
    pthread_mutex_lock(&catraca);  // INICIA LOCKADO. LOOK BUFFET DEVE DAR UNLOCK
    pthread_mutex_lock(&sai_fila); // INICIA LOCKADO. ESTUDANTE DEVE DAR UNLOCK
}

void worker_gate_finalize(worker_gate_t *self)
{
    pthread_join(self->thread, NULL);
    free(self);
    pthread_mutex_destroy(&catraca);
    pthread_mutex_destroy(&sai_fila);
}

void worker_gate_insert_queue_buffet(student_t *student)
{
    // O ESTUDANTE QUE TENTOU SER INSERIDO É O PRIMEIRO DA FILA!
    // Então, caso catraca livre, ele pode ser inserido.
    if (student->_id == globals_get_queue()->_first->_student->_id)
    {
        buffet_t *buffets = globals_get_buffets();
        pthread_mutex_lock(&catraca);
        buffet_queue_insert(&buffets[student->_id_buffet], student);
        pthread_mutex_unlock(&sai_fila); // ELE JÁ ENTROU, POSSO REMOVER ele DA FILA!
    }
}