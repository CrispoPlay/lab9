/*
-----------------------------------------------------------
UNIVERSIDAD DEL VALLE DE GUATEMALA
CC3086 - Programacion de Microprocesadores

Ejercicio 3 - Granularidad del mutex
-----------------------------------------------------------
Descripcion:
Compara dos formas de usar mutex:
1. Mutex grande: bloquea todo el trabajo del hilo.
2. Mutex pequeno: bloquea solo la actualizacion global.
-----------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_HILOS 4
#define LOTES_POR_HILO 20
#define REPORTES_POR_LOTE 50000

long reportes_totales = 0;
pthread_mutex_t mutex;

double tiempo_actual() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1000000000.0;
}

void trabajo_local() {
    volatile long calculo = 0;

    for (long i = 0; i < REPORTES_POR_LOTE; i++) {
        calculo += i % 7;
    }
}

void* tecnico_mutex_grande(void* arg) {
    (void)arg;

    for (int lote = 0; lote < LOTES_POR_HILO; lote++) {
        pthread_mutex_lock(&mutex);

        trabajo_local();
        reportes_totales += REPORTES_POR_LOTE;

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

void* tecnico_mutex_pequeno(void* arg) {
    (void)arg;

    for (int lote = 0; lote < LOTES_POR_HILO; lote++) {
        trabajo_local();

        pthread_mutex_lock(&mutex);
        reportes_totales += REPORTES_POR_LOTE;
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

void ejecutar_prueba(void* (*funcion_hilo)(void*), const char* nombre) {
    pthread_t hilos[NUM_HILOS];

    reportes_totales = 0;
    pthread_mutex_init(&mutex, NULL);

    double inicio = tiempo_actual();

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_create(&hilos[i], NULL, funcion_hilo, NULL);
    }

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], NULL);
    }

    double fin = tiempo_actual();

    pthread_mutex_destroy(&mutex);

    printf("\n%s\n", nombre);
    printf("Reportes esperados: %ld\n", (long)NUM_HILOS * LOTES_POR_HILO * REPORTES_POR_LOTE);
    printf("Reportes obtenidos: %ld\n", reportes_totales);
    printf("Tiempo de ejecucion: %.6f segundos\n", fin - inicio);
}

int main() {
    ejecutar_prueba(tecnico_mutex_grande, "Version A: mutex grande");
    ejecutar_prueba(tecnico_mutex_pequeno, "Version B: mutex pequeno");

    return 0;
}