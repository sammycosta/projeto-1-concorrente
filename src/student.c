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

    /* Inicio um mutex trancado -> talvez usar semáforo para isso. */
    pthread_mutex_init(&self->mutex, NULL);
    pthread_mutex_lock(&self->mutex);

    /* inserindo pq nao achei local que fizesse isso */
    // talvez seja necessário proteger a inserção com um mutex.
    queue_t *fila_de_fora = globals_get_queue();
    queue_insert(fila_de_fora, self);

    pthread_mutex_lock(&self->mutex);      // protejo a entrada dele no buffet
    worker_gate_insert_queue_buffet(self); // só passa quando recebe o unlock

    pthread_mutex_lock(&self->mutex); // só se serve após inserção no buffet terminar
    student_serve(self);

    // terminou de servir:: mutex
    pthread_mutex_t *mutex_served = globals_get_mutex_served();

    pthread_mutex_lock(mutex_served);
    int students_served = globals_get_students_served() + 1;
    globals_set_students_served(students_served);
    pthread_mutex_unlock(mutex_served);

    student_seat(self, tables);
    student_leave(self, tables);

    pthread_exit(NULL);
};

void student_seat(student_t *self, table_t *table)
{
    /* Insira sua lógica aqui */
    printf("estudante %d ENTROU NA FUNÇÃO DE SENTAR\n", self->_id);
    int i = 0;
    int number_of_tables = globals_get_number_of_tables();
    pthread_mutex_t *pegar_cadeira = globals_get_mutex_seats();
    while (i < number_of_tables)
    { // fica no loop olhando as mesas até achar uma com lugar vago
        pthread_mutex_lock(&pegar_cadeira[i]);
        if (table[i]._empty_seats > 0) // garante que o valor está certo (alguma ideia melhor?)
        {
            table[i]._empty_seats--;
            pthread_mutex_unlock(&pegar_cadeira[i]);
            self->_id_buffet = table[i]._id; // salvando a mesa onde antes estava o buffet
            printf("\n !! estudante %d sentou\n", self->_id);
            return; //(não tem variável pra mesa)
        }
        else
        {
            pthread_mutex_unlock(&pegar_cadeira[i]);
            i = (i + 1) % number_of_tables;
        }
    }
}

void student_serve(student_t *self)
{
    /* Insira sua lógica aqui */
    buffet_t *buffet = globals_get_buffets();
    int id_buffet = self->_id_buffet;

    while (self->_buffet_position < 5)
    {
        if (self->_wishes[self->_buffet_position] == 1)
        {
            // LEMBRAR. ESTUDANTES DA MESMA FILA NÃO PODEM PEGAR A MESMA BACIA (USAR L/R)
            // MAS ESTUDANTES DE FILAS DIFERENTES PODEM NA MESMA BACIA!

            // if (buffet[id_buffet]._meal[self->_buffet_position] > 0)
            //{
            sem_wait(&buffet[id_buffet].sem_meals[self->_buffet_position]);
            /*int *val = (int *)(malloc(sizeof(int)));
            sem_getvalue(&buffet[id_buffet].sem_meals[self->_buffet_position], val);
            printf("\nsem buffet %d, %d: %d\n", id_buffet, self->_buffet_position, *val);
            free(val); */

            // lock no mutex da bacia (um mutex pra cada bacia, de cada buffet)
            pthread_mutex_lock(&buffet[id_buffet].mutex_meals[self->_buffet_position]);

            buffet[id_buffet]._meal[self->_buffet_position] -= 1;

            pthread_mutex_unlock(&buffet[id_buffet].mutex_meals[self->_buffet_position]);
            //}
            // talvez ter outro lock pra caso esteja vazio, pra que tente de novo depois?
        }
        msleep(10000); // tempo de se servir
        buffet_next_step(buffet, self);
    }
    printf("~ estudante %d SAIU DO WHILE DE SERVE\n", self->_id);
}

void student_leave(student_t *self, table_t *table)
{
    /* Insira sua lógica aqui */
    msleep(5000); // tempo de comer
    pthread_mutex_t *pegar_cadeira = globals_get_mutex_seats();
    pthread_mutex_lock(&pegar_cadeira[self->_id_buffet]);
    table[self->_id_buffet]._empty_seats++; // libera a cadeira
    pthread_mutex_unlock(&pegar_cadeira[self->_id_buffet]);

    pthread_mutex_destroy(&self->mutex);
    printf("!! Estudante %d foi embora\n", self->_id);
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