#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "config.h"
#include "queue.h"
#include "student.h"
#include "globals.h"
#include "worker_gate.h"
#include "chef.h"
#include "buffet.h"

config_t parse (int argc, char **argv)
{
    int c;
    config_t config = { 50, 2, 10, 5 };

    while ((c = getopt(argc, argv, "n:b:t:s:h")) != -1) {
        switch (c) {
            case 'n':
                config.students = atoi(optarg);
                break;
            case 'b':
                config.buffets = atoi(optarg);
                break;
            case 't':
                config.tables = atoi(optarg);
                break;
            case 's':
                config.seat_per_table = atoi(optarg);
                break;
            case 'h':
                printf("Usage: %s [OPTIONS]\n\nOptions:\n", argv[0]);
                printf("  -n  Number of students (default 50).\n");
                printf("  -b  Number of buffets (default 2).\n");
                printf("  -t  Number of tables (default 10).\n");
                printf("  -s  Number of seats per table (default 5).\n");
                printf("  -h  Prints this help.\n");
            default:
                exit(1);
        }
    }

    return config;
}

/* --------------------------------------------------------- *
 *                                                           *
 *     ATENÇÃO: A FUNÇÃO MAIN NÃO PODERÁ SER ALTERADA!       *
 *                                                           *
 * --------------------------------------------------------- */

chef_t *chef = NULL;
worker_gate_t *worker_gate = NULL;
buffet_t *buffets = NULL;

void _configure_restaurant(int buffet_number)
{
    chef = malloc(sizeof(chef_t));
    worker_gate = malloc(sizeof(worker_gate_t));
    buffets = malloc(sizeof(buffet_t) * buffet_number);
}

int main (int argc, char **argv)
{
    
    /* Inicializa o gerador de números aleatórios. */
    srand(time(NULL));

    /* Configura o parse de argumentos. */
   config_t config = parse(argc, argv);

    printf(RU);
    printf("Students on queue: %d \n",config.students);
    printf("Working buffets: %d \n",config.buffets);
    printf("Tables: %d \n", config.tables);
    printf("Seat per table: %d \n", config.seat_per_table);
    printf(BAR);

    if (config.buffets <= 0 || config.seat_per_table <= 0 || config.students <= 0 || config.tables <= 0)
    {
        printf("[ERROR] - Some variables are invalid. Aborting... \n");
        return 0;
    }

    if((config.seat_per_table * config.tables) < config.buffets * 10){
        printf("[ERROR] - Number of seats must be greater than %d\n", (config.buffets * 10));
        return 0;
    }

    /* Aloca memória para threads da estrutura do restaurante */
    _configure_restaurant(config.buffets);
    
    /* Inicia uma fila global para o RU, com n alunos.*/
    globals_set_queue(queue_init());
    
    /* Inicia uma variável global com o número de estudantes na fila externa*/
    globals_set_students(config.students);

    /* Inicia uma variável global com uma quantidade fixa de mesas */
    globals_set_table(table_init(config.tables,config.seat_per_table));

    /* Inicia o chef */
    chef_init(chef);
    
    /* Inicia os buffet(s)*/
    buffet_init(buffets, config.buffets);

    /* Inicia uma variável global com a referência para os buffets*/
    globals_set_buffets(buffets);

    /* Inicia a catraca e o funcionário que verifica a fila */
    worker_gate_init(worker_gate);

    /*Inicia os estudantes*/
    students_come_to_lunch(config.students);

    /* Finaliza o chef */
    chef_finalize(chef);
    
    /*Finaliza o(s) buffet(s)*/
    buffet_finalize(buffets, config.buffets);

    /* Finaliza a catraca */
    worker_gate_finalize(worker_gate);
    

    return 0;
}