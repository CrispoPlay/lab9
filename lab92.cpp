

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>/*
-----------------------------------------------------------
UNIVERSIDAD DEL VALLE DE GUATEMALA
FACULTAD DE INGENIERIA
DEPARTAMENTO DE CIENCIA DE LA COMPUTACION

CC3086 - Programacion de Microprocesadores

Ejercicio 2 - Acceso concurrente a recurso compartido
-----------------------------------------------------------
Descripcion:
Programa que simula multiples cajeros automaticos realizando
retiros sobre una cuenta compartida sin mecanismos de
sincronizacion.

Cada hilo verifica si existe saldo suficiente antes de aprobar
un retiro. Sin embargo, debido a la ejecucion concurrente,
pueden ocurrir inconsistencias como:

- aprobacion de retiros simultaneos no validos
- saldo negativo
- conteos incorrectos de transacciones

El objetivo es identificar la seccion critica del programa y
analizar por que la validacion del saldo no es suficiente sin
proteccion mediante mutex.
-----------------------------------------------------------
*/

#define NUM_CAJEROS 4
#define INTENTOS 5
#define MONTO_RETIRO 100

int saldo = 500;
int retiros_aprobados = 0;
int retiros_rechazados = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; //1

void* cajero(void* arg) {
    
    int id = *(int*)arg;
    for (int i = 0; i < INTENTOS; i++) {
        usleep(1000); 
        pthread_mutex_lock(&lock);  //2
        if (saldo >= MONTO_RETIRO) {
            //usleep(1000);
            saldo = saldo - MONTO_RETIRO;
            retiros_aprobados++;

            printf("Cajero %d aprobo retiro. Saldo actual: %d\n", id, saldo);
        } 
        
        else {
            retiros_rechazados++;
            printf("Cajero %d rechazo retiro. Saldo insuficiente: %d\n", id, saldo);
        }
        pthread_mutex_unlock(&lock); //3
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t hilos[NUM_CAJEROS];
    int ids[NUM_CAJEROS];

    for (int i = 0; i < NUM_CAJEROS; i++) {
        ids[i] = i + 1;

        if (pthread_create(&hilos[i], NULL, cajero, &ids[i]) != 0) {
            printf("Error al crear el hilo %d\n", i + 1);
            return 1;
        }
    }

    for (int i = 0; i < NUM_CAJEROS; i++) {
        pthread_join(hilos[i], NULL);
    }


    printf("\nResumen final\n");
    printf("Saldo final: %d\n", saldo);
    printf("Retiros aprobados: %d\n", retiros_aprobados);
    printf("Retiros rechazados: %d\n", retiros_rechazados);
    pthread_mutex_destroy(&lock);
    return 0;
}



