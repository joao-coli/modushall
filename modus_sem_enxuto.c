#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define MAX 128

int _heathensCounter = 0;
int _prudesCounter = 0;

pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t _heathensTurn;
sem_t _prudesTurn;

void *prudes() {
    
    pthread_mutex_lock(&_mutex);
    sleep(1);

    _prudesCounter++;
    printf("\n-- PRUDES CHEGOU\nPrudes: %d\t\te\t\tHeathens: %d\n", _prudesCounter, _heathensCounter);
    
    pthread_mutex_unlock(&_mutex);

    sleep(2);
    
    sem_wait(&_prudesTurn);

    pthread_mutex_lock(&_mutex);
    _prudesCounter--;
    
    if(!_prudesCounter) 
        if(_heathensCounter) 
            sem_post(&_heathensTurn);
    
    if(_heathensCounter > _prudesCounter) {
        sem_post(&_heathensTurn);
    } else {sem_post(&_prudesTurn);}

    printf("\nPRUDES ENCERROU\nFaltam %d Prudes\t\te\t\t%d Heathens\n", _prudesCounter, _heathensCounter);
    
    pthread_mutex_unlock(&_mutex);
    pthread_exit(0);
}

void *heathens() {

    pthread_mutex_lock(&_mutex);
    sleep(1);
    
    _heathensCounter++;
    printf("\n-- HEATHENS CHEGOU\nPrudes: %d\t\te\t\tHeathens: %d\n", _prudesCounter, _heathensCounter);
    
    pthread_mutex_unlock(&_mutex);
    
    sleep(1);
    
    sem_wait(&_heathensTurn);
    
    pthread_mutex_lock(&_mutex);
    _heathensCounter--;
    
    if(!_heathensCounter) 
        if(_prudesCounter) 
            sem_post(&_prudesTurn);

    if(_prudesCounter > _heathensCounter) {
        sem_post(&_prudesTurn);
    } else {sem_post(&_heathensTurn);}

    printf("\nHEATHENS ENCERROU\nFaltam %d Prudes\t\te\t\t%d Heathens\n", _prudesCounter, _heathensCounter);

    pthread_mutex_unlock(&_mutex);
    pthread_exit(0);
}

int main() {
    char err_msg[MAX];
    int nHeathens, nPrudes, start;
    pthread_t *_tidPrudes;
    pthread_t *_tidHeathens;
    
    printf("Qual a quantidade Heathens? (no mínimo 1)\n");
    scanf("%d", &nHeathens);
    if(nHeathens < 1) nHeathens = 20;

    printf("Qual a quantidade Prudes? (no mínimo 1)\n");
    scanf("%d", &nPrudes);
    if(nPrudes < 1) nPrudes = 20;
    
    if(nHeathens > nPrudes)
        start = 1;
    else
        start = 0;
    
    _tidHeathens = (pthread_t *) malloc(nHeathens * sizeof(pthread_t));
    _tidPrudes = (pthread_t *) malloc(nPrudes * sizeof(pthread_t));

    //Semáforo de heathens
    if (sem_init(&_heathensTurn, 0, start) < 0) {
        strerror_r(errno, err_msg, MAX);
        printf("Erro em sem_init: %s\n", err_msg);
        exit(1);
    }

    //Semáforo de prudes
    if (sem_init(&_prudesTurn, 0, !start) < 0) {
        strerror_r(errno, err_msg, MAX);
        printf("Erro em sem_init: %s\n", err_msg);
        exit(1);
    }
    
    //-------- Criando Threads ----------
    if(nHeathens >= nPrudes) {
        for(int i = 0; i < nHeathens; i++) {
            pthread_create(&_tidHeathens[i], NULL, &heathens, NULL);
            if(i < nPrudes) pthread_create(&_tidPrudes[i], NULL, &prudes, NULL);
        }
    } else {
         for(int i = 0; i < nPrudes; i++) {
            pthread_create(&_tidPrudes[i], NULL, &prudes, NULL);
            if(i < nHeathens) pthread_create(&_tidHeathens[i], NULL, &heathens, NULL);
        }
    }
    
    while(1);

    return 0;

}
