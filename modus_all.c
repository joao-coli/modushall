#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <string.h> //Uso da função strerror_r
#include <errno.h> //Uso da função strerror

#define MAX 128

#define NEUTRAL 0
#define HEATHENS_RULE 1
#define PRUDES_RULE 2
#define TRANSITION_TO_HEATHENS 3
#define TRANSITION_TO_PRUDES 4

int _heathensCounter = 0;
int _prudesCounter = 0;
int _status = NEUTRAL;
pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t _heathenTurn;
sem_t _prudesTurn;

void *allHeathens(){
    
    while(1) { //A todo momento, já que os valores são sempre incrementados

    sleep(1);
    sem_wait(&_heathenTurn);
    pthread_mutex_lock(&_mutex);
    
    while(_heathensCounter >= _prudesCounter && _heathensCounter > 0){
        _heathensCounter--;
        printf("HEATHENS PASSOU\nFaltam %d Prudes\t\te\t%d Heathens\n", _prudesCounter, _heathensCounter);
        sleep(1);
    }
    sem_post(&_prudesTurn);
    if(_prudesCounter > 0) {
        printf("[%d] > [%d] ", _prudesCounter, _heathensCounter);
        printf("\n[Mais Prudes. Mudando a vez para Prudes]\n\n");
    } else {
       printf("Ninguém está tentando atravessar. Aguardando por novas chegadas\n\n"); 
    }
    pthread_mutex_unlock(&_mutex);
    
    }
    
    pthread_exit(0);
    
}

void *allPrudes(){

    while(1) { //A todo momento, já que os valores são sempre incrementados

    sleep(1);
    sem_wait(&_prudesTurn);
    pthread_mutex_lock(&_mutex);
    
    while(_prudesCounter >= _heathensCounter && _prudesCounter > 0){
        _prudesCounter--;
        printf("Prudes PASSOU\nFaltam %d Prudes\t\te\t%d Heathens\n", _prudesCounter, _heathensCounter);
        sleep(1);
    }
    sem_post(&_heathenTurn);
    if(_heathensCounter > 0) {
        printf("[%d] > [%d] ", _prudesCounter, _heathensCounter);
        printf("\n[Mais Heathens. Mudando a vez para Heathens]\n\n");
    } else {
       printf("Ninguém está tentando atravessar. Aguardando por novas chegadas\n\n"); 
    }
    pthread_mutex_unlock(&_mutex);
    
    }
    
    pthread_exit(0);
    
}

void *randomIncrementer(){
    while(1){
        sleep(4);
        pthread_mutex_lock(&_mutex);
        srand(time(NULL)); 
        int r = rand() % 5;  
	
        if(fator % 2){
            _heathensCounter += r;
            printf("Chegaram mais %d Heathens.\n",r);
        } else {
            printf("Chegaram mais %d Prudes.\n",r);
            _prudesCounter += r;
        }
        pthread_mutex_unlock(&_mutex);
    }
    pthread_exit(0);
}

int main()
{
  char err_msg[MAX];
  int numeroDeHeathens;
  int numeroDePrudes;
  
  pthread_t *_tidPrudes;
  pthread_t *_tidHeathens;
  pthread_t *_tidRandomIncrementer;
  
  printf("Qual a quantidade Heathens? (no mínimo 1)\n");
  scanf("%d", &_heathensCounter);
  if(_heathensCounter < 1) _heathensCounter = 20;

  printf("Qual a quantidade Prudes? (no mínimo 1)\n");
  scanf("%d", &_prudesCounter);
  if(_prudesCounter < 1) _prudesCounter = 20;

  _tidHeathens = (pthread_t *) malloc(sizeof(pthread_t));
  _tidPrudes = (pthread_t *) malloc(sizeof(pthread_t)); 
  _tidRandomIncrementer = (pthread_t *) malloc(sizeof(pthread_t));

  //Heathens tem a preferencia em passar caso o ambiente seja neutro
  if (sem_init(&_heathenTurn, 0, 1) < 0)
  {
    strerror_r(errno, err_msg, MAX);
    printf("Erro em sem_init: %s\n", err_msg);
    exit(1);
  }

  if (sem_init(&_prudesTurn, 0, 0) < 0)
  {
    strerror_r(errno, err_msg, MAX);
    printf("Erro em sem_init: %s\n", err_msg);
    exit(1);
  }
  
  printf("\n------------Inicio-------------\n");
  if(_heathensCounter > _prudesCounter) {
      pthread_create(&_tidHeathens, NULL, &allHeathens, NULL);
      pthread_create(&_tidPrudes, NULL, &allPrudes, NULL);
  } else {
      pthread_create(&_tidPrudes, NULL, &allPrudes, NULL);
      pthread_create(&_tidHeathens, NULL, &allHeathens, NULL);
  }
  
  pthread_create(&_tidRandomIncrementer, NULL, 	&randomIncrementer, NULL);  

  pthread_join(_tidHeathens, NULL);
  pthread_join(_tidPrudes, NULL);
  
  printf("\nTerminou\n");
   
  return 0;
}
