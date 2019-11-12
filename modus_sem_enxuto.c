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

pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER; //Mutex de regiao critica

sem_t _heathensTurn; //Semaforo para os heathens
sem_t _prudesTurn; //Semaforo para os prudes

void *prudes() {
    
    //Chegada na fila
    pthread_mutex_lock(&_mutex);
    sleep(1);

    _prudesCounter++;
    printf("\n-- PRUDES CHEGOU\nPrudes: %d\t\te\t\tHeathens: %d\n", _prudesCounter, _heathensCounter);
    
    pthread_mutex_unlock(&_mutex);

    sleep(1);
    
    //Controle de vez entre prudes e heathens. Impede que um individuo solicite o mutex sem que seja a vez de sua faccao
    sem_wait(&_prudesTurn);

    pthread_mutex_lock(&_mutex);
    _prudesCounter--; //Travessia
    
    if(!_prudesCounter) //Se nao houver mais prudes
        if(_heathensCounter) //E houver heathens
            sem_post(&_heathensTurn); //Libere a execucao dos heathens
    
    if(_heathensCounter > _prudesCounter) { //Caso haja mais heathens do que prudes
        sem_post(&_heathensTurn); //Libere a execucao dos heathens
    } else {sem_post(&_prudesTurn);} //Do contrario, continue executando os prudes

    printf("\nPRUDES ENCERROU\nFaltam %d Prudes\t\te\t\t%d Heathens\n", _prudesCounter, _heathensCounter);
    
    pthread_mutex_unlock(&_mutex);
    pthread_exit(0);
}

void *heathens() {

    //Chegada na fila
    pthread_mutex_lock(&_mutex);
    sleep(1);
    
    _heathensCounter++;
    printf("\n-- HEATHENS CHEGOU\nPrudes: %d\t\te\t\tHeathens: %d\n", _prudesCounter, _heathensCounter);
    
    pthread_mutex_unlock(&_mutex);
    
    sleep(1);
    
    //Controle de vez entre prudes e heathens. Impede que um individuo solicite o mutex sem que seja a vez de sua faccao
    sem_wait(&_heathensTurn);
    
    pthread_mutex_lock(&_mutex);
    _heathensCounter--;
    
    if(!_heathensCounter) //Se nao houver mais heathens
        if(_prudesCounter) //E houver prudes
            sem_post(&_prudesTurn); //Libere a execucao dos prudes

    if(_prudesCounter > _heathensCounter) { //Caso haja mais prudes do que heathens
        sem_post(&_prudesTurn); //Libere a execucao dos prudes
    } else {sem_post(&_heathensTurn);} //Libere a execucao dos heathens

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
    
    //Define o valor inicial de cada semaforo. O semaforo da maior faccao se iniciara com 1 e o da menor com 0. Garante a alternancia para a maioria desde o primeiro caso
    if(nHeathens > nPrudes)
        start = 1; //SemHeathens comeca como 1
    else
        start = 0; //SemPrudes comeca como 1 (recebe !n)
    
    _tidHeathens = (pthread_t *) malloc(nHeathens * sizeof(pthread_t)); //Vetor de threads a serem incializadas para os heathens
    _tidPrudes = (pthread_t *) malloc(nPrudes * sizeof(pthread_t)); //Vetor de threads a serem incializadas para os prudes

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
        //Inicializando as threads
        for(int i = 0; i < nHeathens; i++) {
            pthread_create(&_tidHeathens[i], NULL, &heathens, NULL);
            if(i < nPrudes) pthread_create(&_tidPrudes[i], NULL, &prudes, NULL);
        }
        
        for(int i = 0; i < nPrudes; i++) {
            pthread_join(_tidHeathens[i], NULL);
            if(i < nPrudes) pthread_join(_tidPrudes[i], NULL);
         }
         
    } else {
        //Inicializando as threads
         for(int i = 0; i < nPrudes; i++) {
            pthread_create(&_tidPrudes[i], NULL, &prudes, NULL);
            if(i < nHeathens) pthread_create(&_tidHeathens[i], NULL, &heathens, NULL);
         }
         
         for(int i = 0; i < nPrudes; i++) {
            pthread_join(_tidPrudes[i], NULL);
            if(i < nHeathens) pthread_join(_tidHeathens[i], NULL);
         }
        
    }
    
    printf("-------------------------- FIM --------------------------\n");

    return 0;

}
