#include <stdlib.h>

#include "worker_gate.h"
#include "globals.h"
#include "config.h"
#include "buffet.h"

pthread_mutex_t catraca, sai_fila;

student_t *primeiro_estudante_var;

/* Modifica all_students_entered para TRUE quando não houver estudantes na fila externa */
void worker_gate_look_queue(int *all_students_entered)
{
    int number_students = globals_get_students();
    printf("%d number students\n", number_students);
    *all_students_entered = number_students > 0 ? FALSE : TRUE;
}

/* Atual: pop na queue de estudantes de fora. Não faz nada com ele, só remove; */
void worker_gate_remove_student()
{
    queue_t *fila = globals_get_queue();
    primeiro_estudante_var = queue_remove(fila);
    int number_students = globals_get_students() - 1; // acho que não dá erro por ser um por vez
    globals_set_students(number_students);
    printf("%d REMOVI UM ESTUDANTE\n", globals_get_students());
}

/* Checa todos os buffets, encontra primeiro vazio;
Destrava mutex "catraca" e retorna o lado da fila vazia (LEFT/RIGHT)
Ou retorna N caso não houver local vazio nas filas dos buffets. */
char worker_gate_look_buffet()
{
    buffet_t *buffets = globals_get_buffets();
    int number_of_buffets = globals_get_number_of_buffets();

    if (globals_get_queue()->_first == NULL)
    {
        return 'N';
    }
    for (int i = 0; i < number_of_buffets; i++)
    {
        if (!buffets[i].queue_left[0])
        { // precisa mutex?
            // preenche estudante e libera sua passagem
            worker_gate_remove_student(); // seta primeiro estudante var
            primeiro_estudante_var->_id_buffet = buffets[i]._id;
            primeiro_estudante_var->left_or_right = 'L';
            pthread_mutex_unlock(&catraca);
            return 'L';
        }
        else if (!buffets[i].queue_right[0])
        {
            worker_gate_remove_student(); // seta primeiro estudante var
            primeiro_estudante_var->_id_buffet = buffets[i]._id;
            primeiro_estudante_var->left_or_right = 'L';
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
    printf("ENTREI NO WORKER GATE RUN \n");
    number_students = *((int *)arg);
    all_students_entered = number_students > 0 ? FALSE : TRUE;

    while (all_students_entered == FALSE)
    {
        worker_gate_look_queue(&all_students_entered); // Decide se finaliza thread.
        fila_livre = worker_gate_look_buffet();        // unlock catraca ou não
        if (fila_livre != 'N')
        {
            pthread_mutex_lock(&sai_fila); // Só sai daqui se estudante rodou função insert
        }
        msleep(5000); /* Pode retirar este sleep quando implementar a solução! */
    }

    pthread_exit(NULL);
}

void worker_gate_init(worker_gate_t *self)
{
    pthread_mutex_init(&catraca, NULL);
    pthread_mutex_init(&sai_fila, NULL);
    pthread_mutex_lock(&catraca);  // INICIA LOCKADO. LOOK BUFFET DEVE DAR UNLOCK
    pthread_mutex_lock(&sai_fila); // INICIA LOCKADO. ESTUDANTE DEVE DAR UNLOCK
    init_mutexes();

    printf("entra aqui: worker gate init criou mutexes \n");

    int number_students = globals_get_students();
    pthread_create(&self->thread, NULL, worker_gate_run, &number_students);
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
    if (primeiro_estudante_var == NULL)
    {
        return;
    }
    // student_t *primeiro_estudante = globals_get_queue()->_first->_student;
    // printf("%d id do atual primeiro\n", primeiro_estudante->_id);
    if (student->_id == primeiro_estudante_var->_id)
    {
        printf("entra aqui: é o primeiro estudante!\n");
        buffet_t *buffets = globals_get_buffets();
        pthread_mutex_lock(&catraca);
        buffet_queue_insert(&buffets[student->_id_buffet], student);
        // pthread_mutex_unlock(&sai_fila); // ELE JÁ ENTROU, POSSO REMOVER ele DA FILA!
    }
}