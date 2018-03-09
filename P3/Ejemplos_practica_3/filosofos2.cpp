#include <iostream>
#include <time.h>      // incluye "time"
#include <unistd.h>    // incluye "usleep"
#include <stdlib.h>    // incluye "rand" y "srand"
#include <mpi.h>

#define cogerT  0
#define soltarT 1
#define levantarse 2
#define sentarse 3
#define camarero 4

using namespace std;

void Filosofo( int id, int nprocesos);
void Tenedor ( int id, int nprocesos);
void Camarero();
 
int main( int argc, char** argv )
{
   int rank, size;
   
   srand(time(0));
   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );
   MPI_Comm_size( MPI_COMM_WORLD, &size );
   
   if( size!=11)
   {
      if( rank == 0) 
         cout<<"El numero de procesos debe ser 11" << endl << flush ;
      MPI_Finalize( ); 
      return 0; 
   }
   if(rank == 4)
      Camarero();
   else if((rank%2) == 0)  
      Filosofo(rank,size); // Los pares son Filosofos 
   else 
      Tenedor(rank,size);  // Los impares son Tenedores 
   
   MPI_Finalize( );
   return 0;
}  
// ---------------------------------------------------------------------

void Filosofo( int id, int nprocesos )
{
    	int izq = (id+1) % (nprocesos-1);
    	int der = (id+nprocesos-2) % (nprocesos-1);
	MPI_Status status;
   
   while(1)   {

        // El filosofo pide sentarse
        cout << "Filosofo " << id << " solicita sentarse " <<  endl;
        MPI_Send(NULL, 0, MPI_INT, camarero, sentarse, MPI_COMM_WORLD);

        // El filosofo espera a que le digan que puede sentarse
        MPI_Recv(NULL, 0, MPI_INT, camarero, sentarse, MPI_COMM_WORLD, &status);
        cout << "Filosofo " << id << " se sienta " << endl;

	// El filosofo se sienta

	// Solicita tenedor derecho
	cout <<"Filosofo "<<id<< " coge tenedor der ..." << der << endl << flush;
	MPI_Ssend(NULL, 0, MPI_INT, der, cogerT, MPI_COMM_WORLD);

	// Solicita tenedor izquierdo
	cout << "Filosofo "<<id<< " solicita tenedor izq ..." << izq << endl << flush;
	MPI_Ssend(NULL, 0, MPI_INT, izq, cogerT, MPI_COMM_WORLD);
  
      cout<<"Filosofo "<<id<< " COMIENDO"<<endl<<flush;
      sleep((rand() % 3)+1);  //comiendo
      
      // Suelta el tenedor izquierdo
      cout <<"Filosofo "<<id<< " suelta tenedor izq ..." << izq << endl << flush;
      MPI_Ssend(NULL, 0, MPI_INT, izq, soltarT, MPI_COMM_WORLD);
      
      // Suelta el tenedor derecho
      cout <<"Filosofo "<<id<< " suelta tenedor der ..." << der << endl << flush;
      MPI_Ssend(NULL, 0, MPI_INT, der, soltarT, MPI_COMM_WORLD);
      
      // Piensa (espera bloqueada aleatorio del proceso)
      cout << "Filosofo " << id << " PENSANDO" << endl << flush;

      // el filosofo se levanta
      cout << "Filosofo " << id << " se levanta " << endl;
      MPI_Ssend(NULL, 0, MPI_INT, camarero, levantarse, MPI_COMM_WORLD );

      cout << "Filosofo " << id << " PENSANDO" << endl;
      sleep((rand() % 3)+1);  //pensando
      
      // espera bloqueado durante un intervalo de tiempo aleatorio 
      // (entre una décima de segundo y un segundo)
      usleep( 1000U * (100U+(rand()%900U)) );
 }
}
// ---------------------------------------------------------------------

void Tenedor(int id, int nprocesos)
{
  int buf; 
  MPI_Status status; 
  int Filo;
  
  while( true )
  {
    // Espera un peticion desde cualquier filosofo vecino ...
    MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, cogerT, MPI_COMM_WORLD, &status);
    
    // Recibe la peticion del filosofo ...
    Filo=status.MPI_SOURCE; // Obtiene el rank del filosofo
    
    cout << "Ten. " << id << " recibe petic. de " << Filo << endl << flush;
    
    // Espera a que el filosofo suelte el tenedor...
    MPI_Recv(&Filo, 1, MPI_INT, Filo, soltarT, MPI_COMM_WORLD, &status);
    cout << "Ten. " << id << " recibe liberac. de " << Filo << endl << flush; 
  }
}

void Camarero()
{
    int buf, sentados=0;
    MPI_Status status;
    while (true) {
        if (sentados < 4) // El maximo de filosofos comiendo es 4
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // Puede sentarse o levantarse
        else
            MPI_Probe(MPI_ANY_SOURCE, levantarse, MPI_COMM_WORLD, &status);  // Solo puede levantarse


        if (status.MPI_TAG == sentarse) // se le deja sentarse
        {
            buf=status.MPI_SOURCE;
            MPI_Recv( NULL, 0, MPI_INT, buf, sentarse, MPI_COMM_WORLD,&status);
            sentados++;

            MPI_Send( NULL, 0, MPI_INT, buf, sentarse, MPI_COMM_WORLD);
            cout << "Filosofo " << buf << " se sienta. Hay " << sentados << " filosofos sentados. " << endl;

        }
        if (status.MPI_TAG == levantarse) // Se levanta
        {
            buf=status.MPI_SOURCE;
            MPI_Recv( NULL, 0, MPI_INT, buf, levantarse, MPI_COMM_WORLD,&status);
            sentados--;
            cout << "Filosofo " << buf << " se levanta. Hay " << sentados << " filosofos sentados.  " << endl;

        }
    }

}
// ---------------------------------------------------------------------
