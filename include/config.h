#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdio.h>

/*============================================================================*
 * Configuração                                                               *
 *============================================================================*/



#define FALSE                    0
#define TRUE                     1

typedef struct config {
    unsigned int students;
    unsigned int buffets;
    unsigned int tables;
    unsigned int seat_per_table;
} config_t;

extern config_t config;

/*============================================================================*
 * Macros                                                                     *
 *============================================================================*/

/**
* @brief Imprime um log em modo debug.
*/
#ifndef NDEBUG
    #define plog(...) printf(__VA_ARGS__); fflush(stdout)
#else
    #define plog(...)
#endif

/**
* @brief Serapador.
*/
#define BAR "=============================================================================\n"
#define RU  "=============================== [RU] - CTC ==================================\n"

/*============================================================================*
 * Funções                                                                    *
 *============================================================================*/

/**
* @brief Puts the current thread to sleep.
* 
* @param msecs Time in milliseconds.
* 
* @returns 0 em caso de sucesso e -1 em caso de erro.
*/
extern int msleep(long msec);

#endif /*__CONFIG_H__*/