#include <stdlib.h>

#include "worker_gate.h"
#include "globals.h"
#include "config.h"
#include "buffet.h"

pthread_mutex_t catraca, sai_fila;
buffet_t *buffet_livre; // Preenchida por look_buffet

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
            pthread_mutex_unlock(&catraca);
            buffet_livre = &buffets[i];
            return 'L';
        }
        else if (!buffets[i].queue_right[0])
        {
            pthread_mutex_unlock(&catraca);
            buffet_livre = &buffets[i];
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
        if (fila_livre != 'N')
        {
            // Não existe fila livre: LOCK CATRACA
            pthread_mutex_lock(&catraca); // gambiarra pra não dar deadlock, alguma ideia melhor??
        }

        pthread_mutex_lock(&sai_fila); // POSSO REMOVER O PRIMEIRO ESTUDANTE?? UNLOCK NA INSERT
        worker_gate_remove_student();  // Cuidado com isso, porque posso remover antes dele entregar.

        fila_livre = worker_gate_look_buffet();
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

        pthread_mutex_lock(&catraca);
        buffet_queue_insert(buffet_livre, student);
        pthread_mutex_unlock(&sai_fila); // ELE JÁ ENTROU, POSSO REMOVER ele DA FILA!

        /* Samantha comentando: então, passado desse lock, posso inserir no buffet
        Não sei se era assim para utilizar o sai_fila; apenas ideia. */

        // desisti, não lembro mais o que estava fazendo de manhã...kkkk
        // dúvida: o que tem que acontecer EM ORDEM, e o que não precisa?
    }
}