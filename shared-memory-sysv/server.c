/** 
* @file server.c 
* @author Amir Hossein Sorouri
* @date 28 April 2019 
* @version 0.1 
* @brief An introductory "Shared memory server" loadable kernel 
* module (LKM) that can display a message in the /var/log/kern.log 
* file when the module is loaded and removed. The module can accept 
* an argument when it is loaded -- the name, which appears in the 
* kernel log files. 
*/

// #define _GNU_SOURCE             /* Get definition of MSG_EXCEPT */
/* Kernel Programming */
// #define MODULE
// #define LINUX
// #define __KERNEL__
#include <linux/module.h>   // init_module, cleanup_module //
#include <linux/kernel.h>   // KERN_INFO //
#include <linux/types.h>    // uint64_t //
#include <linux/syscalls.h> // sys_shmget //
#include <linux/kthread.h>  // kthread_run, kthread_stop //
#include <linux/delay.h>    // msleep_interruptible //

// #include "shm_bmk.h"

// #define KERN_INFO   "amir-kernel-info :"
#define TRIALS    5000
#define BUFSIZ    800
#define KEY       9876

// External declarations //
extern long k_shmat( int shmid );
extern long k_semop( int semid, struct sembuf *tsops,
                        unsigned int nsops );


// Function prototypes //
static void handle_message( void );
static int message_ready( void );
static int run_thread( void *data );
static void send_kernel_timing( uint64_t cycles );

// Global variables //
static struct task_struct *shm_task = NULL;
static void *shm                    = NULL;
static int shmid;
static int semid;


/**
 * Read the pentium time stamp counter register.
 * 
 * @return The number of cycles that have elapsed since boot.
 */
__inline__ uint64_t bmk_rdtsc( void )
{
    uint64_t x;
    __asm__ volatile("rdtsc\n\t" : "=A" (x));
    return x;
}


/**
* Called each time a client wishes to benchmark.
*/
static void handle_message( void )
{
    int i;
    char msg[BUFSIZ];
    uint64_t kernel_cycles;
    // uint64_t start;
    // uint64_t stop;
    // uint64_t difference;
    struct sembuf sb = {0, 0, 0};

    kernel_cycles = 0;
    sb.sem_op = -1; // Lock sem 0 //
    if( k_semop( semid, &sb, 1 ) == -1 )
    {
        printk( KERN_INFO "SERVER : Unable to lock sem 0\n" );
        return;
    }

    for( i = 0; i < TRIALS; i++ )
    {
        strncpy( msg, "~Thanks for the message Client", BUFSIZ );
        //printk( KERN_INFO "SERVER : Sending message: %s\n",
        // msg );
        // start = bmk_rdtsc();
        memcpy( shm, msg, BUFSIZ );
        // stop = bmk_rdtsc();
        // difference = stop - start;
        //printk( KERN_INFO "SERVER : Number of cycles: %llu\n",
        //difference );
        // kernel_cycles = kernel_cycles + difference;
    }

    // printk( KERN_INFO "SERVER : Total cycles: %llu\n",
    //         kernel_cycles );
    send_kernel_timing( 52 );
    // send_kernel_timing( kernel_cycles );
    sb.sem_op = 1; // Free sem 0 //
    if( k_semop( semid, &sb, 1 ) == -1 )
    {
        printk( KERN_INFO "SERVER : Unable to free sem 0\n" );
        return;
    }
}

/**
* Checks the shared memory for messages (peeks, but does not remove).
*
* @return TRUE (1) if message is ready, FALSE (0) otherwise.
*/
static int message_ready( void )
{
    if(strncmp( shm, "*", sizeof( char ) ) == 0 )
    {
        return true;
    }
    return false;
}

/**
* The entry point of the kernel thread which is the message benchmark
* server.
*
* @param data Any parameters for the kernel thread.
* @return  The kernel thread exit status.
*/
static int run_thread(void *data)
{
    // union semun arg;
    unsigned long arg = 1;

    semid = sys_semget( KEY, 1, 066 | IPC_CREAT );

    if( semid == -1 )
    {
        printk( KERN_INFO "SERVER : Unable to obtain semid\n" );
        return -1;
    }
    // Note: sys_semctl only handles SETVAL and RMID from kernel
    // space currently

    // arg.val = 1;
    if( sys_semctl( semid, 0, SETVAL, arg ) == -1 )
    {
        printk( KERN_INFO
        "SERVER : Unable to initialize sem 0\n" );
        return -1;
    }
    shmid = sys_shmget( KEY, BUFSIZ, 0666 | IPC_CREAT );

    if( shmid < 0 )
    {
        printk( KERN_INFO "SERVER : Unable to obtain shmid\n" );
        return -1;
    }
    shm = (void *)k_shmat( shmid );
    printk( KERN_INFO "SERVER : Address is %p\n", shm );

    if( !shm )
    {
        printk( KERN_INFO
        "SERVER : Unable to attach to memory\n" );
        return -1;
    }
    strncpy( shm, "~", sizeof( char ) );

    while( !kthread_should_stop() )
    {
        if( message_ready() )
        {
            printk( KERN_INFO "SERVER : Message ready\n" );
            handle_message();
        }
        // ssleep(1000);
        msleep_interruptible( 1000 );    // interruptible sleep, i guess
    }
    return 0;
}

/**
* Pass raw integer timing results to user space where
* floating point operations are allowed.
*
* @param cycles The raw cycles.
*/
static void send_kernel_timing( uint64_t cycles )
{
    memcpy( shm + 1, &cycles, sizeof( uint64_t ) );
}


/**
* Entry point of module execution.
*
* @return The status of the module initialization.
*/
int init_module()
{
    printk( KERN_INFO "SERVER : Initializing shm_server\n" );
    shm_task = kthread_run( run_thread, NULL, "shm_server" );
    return 0;
}

/**
* Exit point of module execution.
*/
void cleanup_module()
{
    int result;
    // union semun arg;
    unsigned long arg = 1;
    printk( KERN_INFO "SERVER : Cleaning up shm_server\n" );
    result = kthread_stop( shm_task );
    if( result < 0 )
    {
        printk( KERN_INFO "SERVER : Unable to stop shm_task\n" );
    }

    result = sys_shmctl( shmid, IPC_RMID, NULL );
    if( result < 0 )
    {
        printk( KERN_INFO
        "SERVER : Unable to remove shared memory from system\n" );
    }
    result = sys_semctl( semid, 0, IPC_RMID, arg );
    if( result == -1 )
    {
        printk( KERN_INFO
        "SERVER : Unable to remove semaphore\n" );
    }
}

// module_init(init_module);
// module_exit(cleanup_module);

///< The license type -- this affects runtime behavior 
MODULE_LICENSE( "GPL" );

///< The author -- visible when you use modinfo 
MODULE_AUTHOR("Amir Hossein Sorouri"); 

///< The description -- see modinfo 
MODULE_DESCRIPTION( "Shared memory benchmark server" );

///< The version of the module 
MODULE_VERSION("0.1"); 