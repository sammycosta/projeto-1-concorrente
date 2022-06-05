#include <stdlib.h>

#include "worker_gate.h"
#include "globals.h"
#include "config.h"

void worker_gate_look_queue(int all_students_entered)
{
    /* Insira aqui sua lógica */
    /* olho a fila de fora, no momento que estiver vazia seto a var
    all_students_entered como TRUE assim a thread para de rodar.
 */
    int number_students = globals_get_queue()->_length;
    all_students_entered = number_students > 0 ? FALSE : TRUE;
}

void worker_gate_remove_student()
{
    /* Insira aqui sua lógica */
    // DEVE ENTRAR UM POR VEZ. DUVIDA SE PRECISA MUTEX
    queue_t fila = globals_get_queue();
    student_t estudante_saindo = queue_remove(fila);

    // worker_gate_insert_queue_buffet(estudante_saindo);
    // essa função acima já é chamada em student_run.
    // eu devo trancar ela e destrancar aqui?
}

void worker_gate_look_buffet()
{
    /* Insira aqui sua lógica */
    // o que faço aqui? checo buffet livre? tranco se nao tem?
    // lock/unlock mutex_catraca
}

void *worker_gate_run(void *arg)
{
    int all_students_entered;
    int number_students;

    number_students = *((int *)arg);
    all_students_entered = number_students > 0 ? FALSE : TRUE;

    while (all_students_entered == FALSE)
    {
        worker_gate_look_queue(all_students_entered);
        worker_gate_look_buffet();
        worker_gate_remove_student();
        msleep(5000); /* Pode retirar este sleep quando implementar a solução! */
    }

    pthread_exit(NULL);
}

void worker_gate_init(worker_gate_t *self)
{
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
    /* Insira aqui sua lógica */
    /* FUNÇÃO CHAMADA EM STUDENT RUN. dar lock aqui? catraca?
     Dúvidas ao colocar estudante no buffet: qual buffet? provavelmente o livre. a função insert buffet do buffet retorna falso se n tiver livre, ok. MAS ONDE LEFT OR RIGHT É SETADA NO ESTUDANTE? AQUI?*/
    // buffet_queue_insert()
}