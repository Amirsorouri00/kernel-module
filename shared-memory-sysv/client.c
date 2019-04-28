#include <stdlib.h>         // exit //
#include <sys/types.h>      // key_t //
#include <sys/ipc.h>        // IPC_CREATE, ftok //
#include <sys/shm.h>        // shmget, ... //
#include <sys/sem.h>        // semget, semop //
#include <stdio.h>          // printf //
#include <string.h>         // strcpy //
#include <stdint.h>         // uint64_t //

// #include "shm_bkm.h"

#define TRIALS    5000
#define KEY       9876

// Processor frequency (floating point) //
const long PROCESSOR_MHZ = 2266.819;

/**
 * Read the pentium time stamp counter register.
 * 
 * @return the number of elapsed cycles since boot.
 */

__inline__ uint64_t bmk_rdtsc(){
    uint64_t x;
    __asm__ volatile ("rdtsc\n\t" : "=A" (x));
    return x;
}

// Function prototypes //
long double benchmark (void *shm, int semid );
void *connect (int shmid );
void disconnect (void *shm);
int getSEM(void);
int getSHM(void);
long double handleKernelTiming( void *shm );

/**
 * Send message to server and perform benchmark.
 * 
 * @param shmid The shared memory handle.
 * @return      The number of cycles for a send.
 */
long double benchmark (void *shm, int semid ){
    int i;
    char msg[BUFSIZ];
    uint64_t start;
    uint64_t stop;
    long double user_cycles;
    uint64_t difference;
    struct sembuf sb = {0,0,0};
    user_cycles = 0.0;

    sb.sem_op = -1; // Lock sem 0 //
    if ( semop( semid, &sb, 1 ) == -1 ){
        perror( "semop");
        exit( -1 );
    }

    strncpy( msg, "* Hello Server", BUFSIZ);

    // printf ( " CLIENT : Sending message: %s\n", msg );

    // start = bmk_rdtsc();
    memcpy( shm, msg, BUFSIZ);
    // stop = bmk_rdtsc();

    difference = 0;
    // difference = stop - start;

    printf( "CLIENT : Initial Start Up: %llu\n", difference );
    for( i = 0; i < TRIALS; i++){
        strncpy( msg, "*How is the weather?", BUFSIZ );

        //printf( "CLIENT : Sending message: %s\n", msg );

        // start =  bmk_rdtsc();
        memcpy( shm, msg, BUFSIZ );
        // stop = bmk_rdtsc();

        // difference = stop - start;
        difference = 0;
        
        //printf( "Number of cycles: %lld\n", difference );

        user_cycles = user_cycles + difference;
    }

    /**
    * Notice we have left an asterisk in first byte
    * of shared memory for kernel to poll for.
    */
   sb.sem_op = 1; // Free sem 0 //
   if( semop( semid, &sb, 1 ) ){
       perror( "semop" );
       exit( -1 );
   }

   user_cycles = user_cycles / (long double)TRIALS;
   return user_cycles;
}

void *connect( int shmid ){
    void *shm = NULL;

    shm =shmat( shmid, NULL, 0 );

    if (!shm )
    {
        perror( "shmat" );
        exit( -1 );
    }

    return shm;
}

void disconnect ( void *shm ){
    int result;

    result = shmdt( shm );

    if (result < 0){
        perror( "shmdt" );
        exit( -1 );
    }
}

/**
 * Connect to the semaphore and obtain the handle.
 * 
 * @return The handle to the semaphore.
 */
int getSEM(){
    int semid;

    semid = semget ( KEY, 1, IPC_CREAT | 0 );

    if ( semid == -1 ){
        perror( "semget" );
        exit( -1 );
    }

    return semid;
}

/**
 * Connect to the shared memory and obtain the handle.
 * 
 * @return The handle to the shared memory.
 */
int getSHM()
{
    int shmid;

    shmid = shmget ( KEY, BUFSIZ, IPC_CREAT | 0666 );

    if ( shmid == -1 ){
        perror( "shmget" );
        exit( -1 );
    }
    return shmid;
}

long double handleKernelTiming( void *shm ){
    uint64_t kernel_cycles;

    while( strncmp( shm, "~", sizeof( char ) ) != 0 )
        sleep( 1 );
    
    kernel_cycles = 0;

    memcpy ( &kernel_cycles, shm + 1, sizeof( uint64_t ) );

    return (long double)kernel_cycles / (long double)TRIALS;
}

/**
 * The entry point of the application.
 * 
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return     The program exit status.
 */
int main( int argc, char  *argv[] ){
    int shmid;
    int semid;
    void *shm;
    long double user_cycles;
    long double user_usecs;
    long double kernel_cycles;
    long double kernel_usecs;

    shmid = getSHM();
    semid = getSEM();
    shm = connect( shmid );

    user_cycles = 0.0;
    kernel_cycles = 0.0;
    // user_cycles = benchmark( shm, semid );
    // kernel_cycles = handleKernelTiming( shm );

    user_usecs = user_cycles / PROCESSOR_MHZ;
    kernel_usecs = kernel_cycles / PROCESSOR_MHZ;

    printf( "CLIENT : Shared memory benchmark\n" );
    printf( "CLIENT : Message size: %d bytes\n", BUFSIZ );
    printf( "CLIENT : Number of iterations: %d\n", TRIALS );
    printf( "CLIENT : User cycles: %llf\n", user_cycles );
    // printf( "CLIENT : User nanoseconds: %llf\n",
    //  user_usecs * 1000.0 );
    printf( "CLIENT : Kernel cycles: %llf\n", kernel_cycles );
    // printf( "CLIENT : Kernel nanoseconds: %llf\n",
    //  kernel_usecs * 1000.0 );
    disconnect( shm );
    return 0;
}
