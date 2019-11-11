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


//Executem o ./script.sh pra fazer os testes
//Erro no turn. se quando os dois são zero e ainda tem thread pra entrar
//Se post nos dois turns, está dando erro dos dois entrarem pelo wait, portanto, assim que o mutex
//estiver 1, o lado oposto avança independente da quantidade de cada lado.
//COMO RESOLVER??!!!

//Novas funções (Luid)
//Mantive as antigas abaixo, porque não sei se isso está certo.
void *heathens(){
  //lógica tomada: cada thread é uma "pessoa", ou seja, 
  //quando a thread é chamada significa que uma pessoa chegou, 
  //logo, a incrementação independe se é a vez dele ou não

    //  printf("\n+1 HEATENS\n");
    _heathensCounter++;

    sem_wait(&_heathenTurn);
    
    pthread_mutex_lock(&_mutex);


    _heathensCounter--;
    printf("HEATHENS PASSOU\nFaltam %d Prudes\t\te\t%d Heathens\n", _prudesCounter, _heathensCounter);

    if(_prudesCounter > _heathensCounter){
        sem_post(&_prudesTurn);
        printf("[%d] > [%d] ", _prudesCounter, _heathensCounter);
        printf("\n[Mais Prudes. Mudando a vez para Prudes]\n\n");
        // _status = PRUDES_RULE;
    } else {
        if(_heathensCounter){
          sem_post(&_heathenTurn);           
        }else{
           printf("\n[Acabou os Heathens. Mudando a vez para Prudes]\n\n");
            // _status = NEUTRAL;
            //sem_post(&_heathenTurn);
            sem_post(&_prudesTurn);
        }
    }


    pthread_mutex_unlock(&_mutex);
    pthread_exit(0);

}

void *allHeathens(){
    
    while(1) {

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

    while(1) {

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

void *prudes(){
    sleep(1);
    _prudesCounter++;
    // printf("\n+1 PRUDE\n");

    sem_wait(&_prudesTurn);

    pthread_mutex_lock(&_mutex);

    
    // printf("PRUDES PASSANDO\nFaltam %d Prudes\t\te\t%d Heathens\n", _prudesCounter, _heathensCounter);


    _prudesCounter--;
    printf("PRUDES PASSOU\nFaltam %d Prudes\t\te\t%d Heathens\n", _prudesCounter, _heathensCounter);
    
    
    if(_heathensCounter > _prudesCounter){        
        sem_post(&_heathenTurn);
        printf("[%d] > [%d] \n", _heathensCounter,_prudesCounter);
        printf("\n[Mais Heathens. Mudando a vez para Heathens]\n\n");
        // _status = HEATHENS_RULE;
    } else{
        if(_prudesCounter){
           sem_post(&_prudesTurn);
        }else{
           printf("\n[Acabou os Prudes. Mudando a vez para Heathens]\n\n");
            // _status = NEUTRAL;
            sem_post(&_heathenTurn);
            //sem_post(&_prudesTurn);
        } 
    }


    pthread_mutex_unlock(&_mutex);
    pthread_exit(0);
    

}

void *randomIncrementer(){
    while(1){
        sleep(3);
        pthread_mutex_lock(&_mutex);
        srand(time(NULL)); 
        int fator = rand() % 198 + 2;
        usleep(fator*1000);
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

  while(1);
  
  printf("\nTerminou\n");
   
  return 0;
}
