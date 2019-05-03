#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <signal.h>
#define SHM_NAME "shah_aniket"
#define PAGE_SIZE getpagesize()
// ADD NECESSARY HEADERS

//Add the structure over here
typedef struct {
    int pid;
    char birth[25];
    char clientString[10];
    int elapsed_sec;
    double elapsed_msec;
} stats_t;



int fd_shm = -1;
// Mutex variables
pthread_mutex_t* mutex;
pthread_mutexattr_t mutexAttribute;

void* point;


void exit_handler(int sig) 
{
    // ADD
    //call unmap and unlink
    munmap(point,PAGE_SIZE);
    close(fd_shm);
    shm_unlink(SHM_NAME);    
	exit(0);
}

int main(int argc, char *argv[]) 
{
    // ADD
    //setup using sigaction systemcall, servers
    struct sigaction act;

    act.sa_handler = exit_handler;
    sigaction(SIGINT,&act,NULL);
    sigaction(SIGTERM,&act,NULL);
    sigaction(SIGKILL,&act,NULL);
    sigaction(SIGTSTP,&act,NULL);
    // pthread_mutex_t* point;
      stats_t *p;
     

	
	// Creating a new shared memory segment
	 fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0660);
	if(fd_shm==-1)
	{
	   exit(0);
	}
	//int pageSize = getpagesize(void);
	
	//Add ftruncate with get page size system call
	if(ftruncate(fd_shm,PAGE_SIZE))
	{
		exit(0);
	}

	//Add Mmap 	
	  
     point = (void*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
     if(point==MAP_FAILED)
     {
     	exit(0);
     }

	mutex = (pthread_mutex_t*)point;
    // Initializing mutex
	pthread_mutexattr_init(&mutexAttribute);
	pthread_mutexattr_setpshared(&mutexAttribute, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(mutex, &mutexAttribute);

	//Intitilze all the segments

	int numberOfSegments = PAGE_SIZE / 64 ;
	
	int counter =0;
	pthread_mutex_lock(mutex);
	for(int i=1; i<numberOfSegments; i++)
	{
		p = (stats_t*)(point + i*64);
		p->pid = -1 ;
	}
	pthread_mutex_unlock(mutex);
    while (1) 
	{
		// ADD
		sleep(1);
		for(int i=1 ; i < numberOfSegments ; i++)
		{
			//Declare lock
			p = (stats_t*)(point + i*64);
			if(p->pid != -1)
			{
				//Do the various computing 
				printf("%d, pid : %d, birth : %s, elapsed : %d s %.4f ms, %s\n",
					counter,p->pid,p->birth,p->elapsed_sec,p->elapsed_msec,
					p->clientString);


			}
		}
		counter = counter + 1;
		
       
    }

    return 0;
}

