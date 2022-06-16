#include <stdlib.h>

#include "worker_gate.h"
#include "globals.h"
#include "config.h"
#include "buffet.h"

student_t *primeiro_estudante_var; // para remove&look conversarem

/* Modifica all_students_entered para TRUE quando não houver estudantes na fila externa */
void worker_gate_look_queue(int *all_students_entered)
{
    int number_students = globals_get_students();
    *all_students_entered = number_students > 0 ? FALSE : TRUE;
}

/* Atual: pop na queue de estudantes de fora. Set primeiro_estudante_var */
void worker_gate_remove_student()
{
    queue_t *fila = globals_get_queue();
    primeiro_estudante_var = queue_remove(fila);
    int number_students = globals_get_students() - 1; // acho que não dá erro por ser um por vez
    globals_set_students(number_students);
    printf("%d estudantes até agora -> REMOVI O ESTUDANTE %d \n", globals_get_students(), primeiro_estudante_var->_id);
}

/* Checa todos os buffets, encontra primeiro vazio;
Destrava mutex "catraca" e retorna o lado da fila vazia (LEFT/RIGHT)
Ou retorna N caso não houver local vazio nas filas dos buffets. */
char worker_gate_look_buffet()
{
    buffet_t *buffets = globals_get_buffets();
    int number_of_buffets = globals_get_number_of_buffets();

    if (globals_get_queue()->_first == NULL)
        return 'N';

    for (int i = 0; i < number_of_buffets; i++)
    {
        if (!buffets[i].queue_left[0])
        { // precisa mutex?
            // preenche estudante e libera sua passagem

            worker_gate_remove_student(); // seta primeiro estudante var
            primeiro_estudante_var->_id_buffet = buffets[i]._id;
            primeiro_estudante_var->left_or_right = 'L';
            pthread_mutex_unlock(&primeiro_estudante_var->mutex);
            return 'L';
        }
        else if (!buffets[i].queue_right[0])
        {
            worker_gate_remove_student(); // seta primeiro estudante var
            primeiro_estudante_var->_id_buffet = buffets[i]._id;
            primeiro_estudante_var->left_or_right = 'R';
            pthread_mutex_unlock(&primeiro_estudante_var->mutex);
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
    printf("ENTREI NO WORKER GATE RUN \n");
    number_students = *((int *)arg);
    all_students_entered = number_students > 0 ? FALSE : TRUE;

    while (all_students_entered == FALSE)
    {
        worker_gate_look_queue(&all_students_entered); // Decide se finaliza thread.
        fila_livre = worker_gate_look_buffet();        // unlock catraca ou não
        if (fila_livre != 'N')
        {
            // Look_buffet setou alguém para entrar na fila do buffet. Devo esperar essa pessoa terminar de entrar (unlockado na buffet_queue_insert);
            pthread_mutex_t *sai_fila = globals_get_mutex_gate();
            printf("esperando unlock do sai fila\n ");
            pthread_mutex_lock(sai_fila);
            printf("passei aqui: último estudante foi inserido\n");
        }

        // msleep(5000); /* Pode retirar este sleep quando implementar a solução! */
    }

    pthread_exit(NULL);
}

void worker_gate_init(worker_gate_t *self)
{

    init_mutexes();

    printf("entra aqui: worker gate init criou mutexes \n");

    int number_students = globals_get_students();
    pthread_create(&self->thread, NULL, worker_gate_run, &number_students);
}

void worker_gate_finalize(worker_gate_t *self)
{
    pthread_join(self->thread, NULL);
    free(self);
}

void worker_gate_insert_queue_buffet(student_t *student)
{
    // garantidamente só chamada após look_queue permitir
    printf("entra aqui: %d, no buffet %d na fila %d\n", student->_id, student->_id_buffet, student->left_or_right);
    buffet_t *buffets = globals_get_buffets();
    buffet_queue_insert(&buffets[student->_id_buffet], student);
}