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

// Contadores iniciais do numero de integrantes das faccoes
int _heathensCounter = 0;
int _prudesCounter = 0;

// Criando o mutex e as barreiras de cada grupo
pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t hbarrier;
pthread_barrier_t pbarrier;

// Parte referente ao grupo de Heathens
void *heathens(){
  // aumenta o numero de Heathens na fila
  _heathensCounter++;
  printf("\n-- Chegou Heathens\nPrudes: %d\t\te\t\tHeathens: %d\n", _prudesCounter, _heathensCounter);
  
  // Coloca a nova Thread na barreira
   pthread_barrier_wait(&hbarrier);
  // Solicita a vez para atravessar
   pthread_mutex_lock(&_mutex);

  // Atravessa o caminho
  _heathensCounter--;
  printf("\n                               HEATHENS PASSOU\nFaltam %d Prudes\t\te\t\t%d Heathens\n", _prudesCounter, _heathensCounter);

  // Espera 1 segundo
  sleep(1);
  // Libera o acesso ao caminho
  pthread_mutex_unlock(&_mutex);

  pthread_exit(0);

}

// Parte referente ao grupo de Prudes
void *prudes(){
  // aumenta o numero de Prudes na fila
  _prudesCounter++;
  printf("\n-- Chegou Prudes\nPrudes: %d\t\te\t\tHeathens: %d\n", _prudesCounter, _heathensCounter);

  // Coloca a nova Thread na barreira
  pthread_barrier_wait(&pbarrier);
  // Solicita a vez para atravessar
  pthread_mutex_lock(&_mutex);
	
  // Atravessa o caminho
  _prudesCounter--;
  printf("\nPRUDES PASSOU\nFaltam %d Prudes\t\te\t\t%d Heathens\n", _prudesCounter, _heathensCounter);

  // Espera 1 segundo
  sleep(1);
  // Libera o acesso ao caminho
  pthread_mutex_unlock(&_mutex);

  pthread_exit(0);
}

int main()
{
  int i;
  
  char err_msg[MAX];
  int numeroDeHeathens, numeroDePrudes;

  // Cria os ponteiros para as Threads de cada grupo
  pthread_t *_tidPrudes;
  pthread_t *_tidHeathens;

  // Solicita ao usuario a quantidade de Heathens que ira atravessar o caminho
  printf("Qual a quantidade Heathens? (no mínimo 1)\n");
  scanf("%d", &numeroDeHeathens);
  if(numeroDeHeathens < 1) numeroDeHeathens = 20;

  // Solicita ao usuario a quantidade de Prudes que ira atravessar o caminho
  printf("Qual a quantidade Prudes? (no mínimo 1)\n");
  scanf("%d", &numeroDePrudes);
  if(numeroDePrudes < 1) numeroDePrudes = 20;
	
  // Cria constantes com os valores do numero de Heathens e Prudes
  const int nH = numeroDeHeathens;
  const int nP = numeroDePrudes;

  // Instancia as barreiras com o menor valor dentre as constantes acima como limite
  if(nH > nP){
  pthread_barrier_init(&hbarrier, NULL, nP);
  pthread_barrier_init(&pbarrier, NULL, nP);
  }
  else{
  pthread_barrier_init(&hbarrier, NULL, nH);
  pthread_barrier_init(&pbarrier, NULL, nH);
  }	

  // Instancia um vetor de ponteiro de Threads
  pthread_t ids[THREAD_COUNT];
  int short_ids[THREAD_COUNT];

  // Aloca memoria para as Threads de cada grupo
  _tidHeathens = (pthread_t *) malloc(numeroDeHeathens * sizeof(pthread_t));
  _tidPrudes = (pthread_t *) malloc(numeroDePrudes * sizeof(pthread_t));

  printf("\n--------------------- Inicio ----------------------\n");

  // Verificacoes para dar suporte a diferentes numeros de threads
  // Cria as novas threads a partir dessas verificacoes
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

  // Destroi as barreiras se tiver acabado a travessia de algum dos grupos
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
