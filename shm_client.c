#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include<string.h>

#define SHM_NAME "shah_aniket"
#define PAGE_SIZE getpagesize()

// ADD NECESSARY HEADERS
typedef struct {
    int pid;
    char birth[25];
    char clientString[10];
    int elapsed_sec;
    double elapsed_msec;
} stats_t;

 stats_t *p;
// Mutex variables
pthread_mutex_t* mutex;

void exit_handler(int sig) {
    // ADD


    // critical section begins
	pthread_mutex_lock(mutex);

    // Client leaving; needs to reset its segment
    p->pid= -1; 

	//printf("%s",p->clientString);
	pthread_mutex_unlock(mutex);
	// critical section ends
	
    exit(0);
}

int main(int argc, char *argv[]) {
	// ADD 



	 void* point; 

     time_t curtime; 
     struct timeval start;
     struct timeval end;
     int flag=0;
    // char *str = NULL;
     int elapsed = 0;
     double elapsed_milli = 0;


    struct sigaction act;
    act.sa_handler = exit_handler;
    sigaction(SIGINT,&act,NULL);
    sigaction(SIGTERM,&act,NULL);
    sigaction(SIGKILL,&act,NULL);
    //gettimeofday(&start,NULL);
      
	
     int fd_shm = shm_open(SHM_NAME, O_RDWR , 0660);
	if(fd_shm < 0)
	{
		exit(0);
	}
	// int pageSize = getpagesize(void);
	 int numberOfSegments = PAGE_SIZE / 64 ;

	point = (void*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0); 
	//mutex = (pthread_mutex_t*)point;
     if(point==MAP_FAILED)
     {
     	exit(0);
     } 
	 mutex = (pthread_mutex_t*)point;
	
    // critical section begins
   pthread_mutex_lock(mutex);
        
	// client updates available segment

    	//Loop so you can find a place where to update the client
        for(int i=1 ; i<numberOfSegments ; i++)
		{
			//Declare lock
			//pthread_mutex_lock(mutex);
			p = (stats_t*)(point + i*64);
			
			if(p->pid == -1)
			{
				flag=1;
				p->pid = getpid();
/*
				gettimeofday(&start,NULL);
				
				time(&curtime);
        			strcpy(p->birth,ctime(&curtime));
        //str = strdup(argv[1]);
        			strcpy(p->clientString,argv[1]);
        			p->elapsed_sec = 0;
        			p->elapsed_msec = 0;
				*/
					
				  // pthread_mutex_unlock(mutex);
				   break;
			
			}
			//pthread_mutex_unlock(mutex);
		}

		if(flag==0)
		{
			//printf("All the segments are full, max clients reached");
			pthread_mutex_unlock(mutex);
			exit(0);
		}


		
	pthread_mutex_unlock(mutex);
    // critical section ends
  	gettimeofday(&start,NULL);
    	time(&curtime);
	strcpy(p->birth,ctime(&curtime));
	p->birth[strlen(p->birth)-1]= '\0';
	//str = strdup(argv[1]);
	if(argv[1] == NULL)
	{
		exit(0);
	}
	if(strlen(argv[1]) > 9)
	{
		exit(0);
	}
	strcpy(p->clientString,argv[1]);
	p->elapsed_sec = 0;
	p->elapsed_msec = 0;
	//p->clientString = str;
        
	while (1) {

		//pthread_mutex_lock(mutex);

		gettimeofday(&end,NULL);
		
		elapsed = end.tv_sec - start.tv_sec;
		elapsed_milli = (end.tv_usec - start.tv_usec)/1000.0f;

		p->elapsed_sec = elapsed ;
		p->elapsed_msec = elapsed_milli;

		//pthread_mutex_unlock(mutex);


		 sleep(1);

		printf("Active clients :");
		//Loop to just print active clients
		for(int i=1 ; i<numberOfSegments ; i++)
		{
			 stats_t* curr = (stats_t*)(point + i*64);
			
			if(curr->pid != -1)
			{
				// Print active clients
				printf(" %d",curr->pid);

			}
		}
		printf("\n");//counter = counter + 1;
		// ADD
        
       
		
    }

    return 0;
}

