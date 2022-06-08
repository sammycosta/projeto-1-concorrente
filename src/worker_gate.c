#include <stdlib.h>

#include "worker_gate.h"
#include "globals.h"
#include "config.h"
#include "buffet.h"

pthread_mutex_t catraca, sai_fila;

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
    queue_t *fila = globals_get_queue(); //só botei * pra parar de dar erro de sintaxe
    student_t *estudante_saindo = queue_remove(fila); 

    // worker_gate_insert_queue_buffet(estudante_saindo);
    // essa função acima já é chamada em student_run.
    // eu devo trancar ela e destrancar aqui?
}

char worker_gate_look_buffet()
{
    /* Insira aqui sua lógica */
    // o que faço aqui? checo buffet livre? tranco se nao tem?
    // lock/unlock mutex_catraca
    buffet_t *buffets = globals_get_buffets();
    for (int i = 0; i < config.buffets; i++) { 
        if (!buffets[i].queue_left[0]) { //precisa mutex?
            pthread_mutex_unlock(&catraca);
            return 'L';
        } else if (!buffets[i].queue_right[0]) {
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
        worker_gate_look_queue(all_students_entered);
        if (fila_livre != 'N') {
            pthread_mutex_lock(&catraca); // gambiarra pra não dar deadlock, alguma ideia melhor??
        }
        fila_livre = worker_gate_look_buffet();
        worker_gate_remove_student();
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
    /* Insira aqui sua lógica */
    /* FUNÇÃO CHAMADA EM STUDENT RUN. dar lock aqui? catraca?
     Dúvidas ao colocar estudante no buffet: qual buffet? provavelmente o livre. a função insert buffet do buffet retorna falso se n tiver livre, ok. MAS ONDE LEFT OR RIGHT É SETADA NO ESTUDANTE? AQUI?*/
    // buffet_queue_insert()
    if (student->_id == globals_get_queue()->_first) {
        pthread_mutex_lock(&catraca);
        //desisti, não lembro mais o que estava fazendo de manhã...kkkk
        //dúvida: o que tem que acontecer EM ORDEM, e o que não precisa?
    }
}