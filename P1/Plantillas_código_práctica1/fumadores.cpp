// *****************************************************************************
//
// Prácticas de SCD. Práctica 1.
// Plantilla de código para el ejercicio de los fumadores
//
// *****************************************************************************

#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>      // incluye "time(....)"
#include <unistd.h>    // incluye "usleep(...)"
#include <stdlib.h>    // incluye "rand(...)" y "srand"

using namespace std ;
sem_t papel,tabaco,cerillas,estanquero,mutex;

// ---------------------------------------------------------------------
// introduce un retraso aleatorio de duración comprendida entre
// 'smin' y 'smax' (dados en segundos)

void retraso_aleatorio( const float smin, const float smax )
{
  static bool primera = true ;
  if ( primera )        // si es la primera vez:
  {  srand(time(NULL)); //   inicializar la semilla del generador
     primera = false ;  //   no repetir la inicialización
  }
  // calcular un número de segundos aleatorio, entre {\ttbf smin} y {\ttbf smax}
  const float tsec = smin+(smax-smin)*((float)random()/(float)RAND_MAX);
  // dormir la hebra (los segundos se pasan a microsegundos, multiplicándos por 1 millón)
  usleep( (useconds_t) (tsec*1000000.0)  );
}

// ----------------------------------------------------------------------------
// función que simula la acción de fumar, como un retardo aleatorio de la hebra.
// recibe como parámetro el numero de fumador
// el tiempo que tarda en fumar está entre dos y ocho décimas de segundo.

void fumar( int num_fumador )
{
   cout << "\t\t\t\tFumador número " << num_fumador << ": comienza a fumar." << endl << flush ;
   retraso_aleatorio( 0.2, 0.8 );
   cout << "\t\t\t\tFumador número " << num_fumador << ": termina de fumar." << endl << flush ;
}
// ----------------------------------------------------------------------------

void *funcion_fumador(void *id_fumador) {
        unsigned long fumador = (unsigned long) id_fumador;
      
	if(fumador==0) {
		while(true) {
			sem_wait(&cerillas);
			sem_wait(&mutex);
			fumar(0);
			sem_post(&mutex);
			sem_post(&estanquero);
		}
	}else if(fumador==1) {
		while(true) {
			sem_wait(&tabaco);
			sem_wait(&mutex);
			fumar(1);
			sem_post(&mutex);
			sem_post(&estanquero);
		}
	}else{
		while(true) {
			sem_wait(&papel);
			sem_wait(&mutex);
			fumar(2);
			sem_post(&mutex);
			sem_post(&estanquero);
		}
	}
}

void *funcion_estanquero(void *id_estanquero){
	int ingrediente;
	while(true) {
        	sem_wait(&estanquero);
		ingrediente=(int)random()%3;
		if(ingrediente==0) {
			sem_wait(&mutex);
			cout << "El estanquero produce papel" << endl << flush;
			sem_post(&mutex);
			sem_post(&papel);
		}else if(ingrediente==1) {
			sem_wait(&mutex);
			cout << "El estanquero produce tabaco" << endl << flush;
			sem_post(&mutex);
			sem_post(&tabaco);
		}else {
			sem_wait(&mutex);
			cout << "El estanquero produce cerillas" << endl << flush;
			sem_post(&mutex);
			sem_post(&cerillas);
		}
	}
}

int main() {

  srand( time(NULL) );

  pthread_t h_fumador,h_estanquero;

  sem_init(&estanquero,0,1);
  sem_init(&papel,0,0);
  sem_init(&cerillas,0,0);
  sem_init(&tabaco,0,0);
  sem_init(&mutex,0,1);


  for(unsigned long i =0; i<3; i++)
	pthread_create(&h_fumador,NULL,funcion_fumador,(void *) i);

  pthread_create(&h_estanquero,NULL,funcion_estanquero,NULL);

  pthread_exit(NULL);

	

  return 0 ;
}

