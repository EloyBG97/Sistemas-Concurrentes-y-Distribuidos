// *****************************************************************************
//
// Prácticas de SCD. Práctica 1.
// Plantilla de código para el ejercicio del productor-consumidor con
// buffer intermedio.
//
// *****************************************************************************

#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // necesario para {\ttbf usleep()}
#include <stdlib.h> // necesario para {\ttbf random()}, {\ttbf srandom()}
#include <time.h>   // necesario para {\ttbf time()}

using namespace std ;

// ---------------------------------------------------------------------
// constantes configurables:

const unsigned  num_items  = 40 ,    // numero total de items que se producen o consumen
                tam_vector = 10 ;    // tamaño del vector, debe ser menor que el número de items

  
  int buffer[tam_vector],
      *primera_libre=buffer;

  sem_t mutex, escribir,leer;

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

// ---------------------------------------------------------------------
// función que simula la producción de un dato

unsigned producir_dato()
{
  static int contador = 0 ;
  contador = contador + 1 ;
  retraso_aleatorio( 0.1, 0.5 );
  cout << "Productor : dato producido: " << contador << endl << flush ;
     primera_libre++;
  return contador ;
}
// ---------------------------------------------------------------------
// función que simula la consumición de un dato

void consumir_dato( int dato )
{
   retraso_aleatorio( 0.1, 1.5 );
   cout << "Consumidor:                              dato consumido: " << dato << endl << flush ;
   primera_libre--;
}
// ---------------------------------------------------------------------
// función que ejecuta la hebra del productor

void * funcion_productor( void * )
{
  for( unsigned i = 0 ; i < num_items ; i++ )
  {
    sem_wait(&escribir);
	
    sem_wait(&mutex);
    *primera_libre = producir_dato();
    sem_post(&mutex);

    cout << "Productor : dato insertado: " << *(primera_libre -1) <<  endl << flush ;

    sem_post(&leer);
  }
  return NULL ;
}
// ---------------------------------------------------------------------
// función que ejecuta la hebra del consumidor

void * funcion_consumidor( void * )
{
  for( unsigned i = 0 ; i < num_items ; i++ )
  {
    sem_wait(&leer);

    cout << "Consumidor:                              dato extraído : " << *(primera_libre-1) << endl << flush ;
    
    sem_wait(&mutex);
    consumir_dato( *(primera_libre-1) ) ;
    sem_post(&mutex);

    sem_post(&escribir);
  
  }
  return NULL ;
}
//----------------------------------------------------------------------

int main()
{

   pthread_t hebra_escritora, hebra_lectora ;

   sem_init( &escribir, 0, 10 ); // inicialmente se puede escribir
   sem_init( &leer, 0, 0 ); // inicialmente no se puede leer
   sem_init( &mutex, 0, 1 );
    
  
   pthread_create( &hebra_escritora, NULL, funcion_productor, NULL );
   pthread_create( &hebra_lectora,   NULL, funcion_consumidor,     NULL );


   pthread_exit(NULL);
   return 0 ;
}
