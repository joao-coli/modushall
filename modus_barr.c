#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define MAX 128
#define THREAD_COUNT 4

int _heathensCounter = 0;
int _prudesCounter = 0;

pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t hbarrier;
pthread_barrier_t pbarrier;


void *heathens(){

  _heathensCounter++;
  printf("\n-- Chegou Heathens\nPrudes: %d\t\te\t\tHeathens: %d\n", _prudesCounter, _heathensCounter);

   pthread_barrier_wait(&hbarrier);
   pthread_mutex_lock(&_mutex);

 
  _heathensCounter--;
  printf("\n                               HEATHENS PASSOU\nFaltam %d Prudes\t\te\t\t%d Heathens\n", _prudesCounter, _heathensCounter);

  sleep(1);
  pthread_mutex_unlock(&_mutex);




  pthread_exit(0);

}

void *prudes(){

  _prudesCounter++;
  printf("\n-- Chegou Prudes\nPrudes: %d\t\te\t\tHeathens: %d\n", _prudesCounter, _heathensCounter);

  pthread_barrier_wait(&pbarrier);
  pthread_mutex_lock(&_mutex);
	

  _prudesCounter--;
  printf("\nPRUDES PASSOU\nFaltam %d Prudes\t\te\t\t%d Heathens\n", _prudesCounter, _heathensCounter);

  sleep(1);
  pthread_mutex_unlock(&_mutex);

  pthread_exit(0);
}

int main()
{
  int i;
  
  char err_msg[MAX];
  int numeroDeHeathens, numeroDePrudes;

  pthread_t *_tidPrudes;
  pthread_t *_tidHeathens;

  srand(time(NULL));

  

  printf("Qual a quantidade Heathens? (no mínimo 1)\n");
  scanf("%d", &numeroDeHeathens);
  if(numeroDeHeathens < 1) numeroDeHeathens = 20;

  printf("Qual a quantidade Prudes? (no mínimo 1)\n");
  scanf("%d", &numeroDePrudes);
  if(numeroDePrudes < 1) numeroDePrudes = 20;
	
  const int nH = numeroDeHeathens;
  const int nP = numeroDePrudes;

  if(nH > nP){
  pthread_barrier_init(&hbarrier, NULL, nP);
  pthread_barrier_init(&pbarrier, NULL, nP);
  }
  else{
  pthread_barrier_init(&hbarrier, NULL, nH);
  pthread_barrier_init(&pbarrier, NULL, nH);
  }	

  pthread_t ids[THREAD_COUNT];
  int short_ids[THREAD_COUNT];

  _tidHeathens = (pthread_t *) malloc(numeroDeHeathens * sizeof(pthread_t));
  _tidPrudes = (pthread_t *) malloc(numeroDePrudes * sizeof(pthread_t));

  printf("\n--------------------- Inicio ----------------------\n");

  //Verificações para dar suporte a diferentes números de threads

  if(numeroDeHeathens >= numeroDePrudes){

    for (int i = 0; i < numeroDeHeathens; i++){
      pthread_create(&_tidHeathens[i], NULL, &heathens, NULL);
      sleep(1);
      if(i<numeroDePrudes) pthread_create(&_tidPrudes[i], NULL, &prudes, NULL);
    }
  }
  else{
      for (int i = 0; i < numeroDePrudes; i++){
        pthread_create(&_tidPrudes[i], NULL, &prudes, NULL);
        sleep(1);
        if(i<numeroDeHeathens) pthread_create(&_tidHeathens[i], NULL, &heathens, NULL);
      }
  }

  while(1){
   sleep(1);
  	if(_heathensCounter == 0 || _prudesCounter == 0){
  		pthread_barrier_destroy(&hbarrier);
  		pthread_barrier_destroy(&pbarrier);
  	}
  }

  printf("\n---------------------- Fim -----------------------\n");

  return 0;
}
