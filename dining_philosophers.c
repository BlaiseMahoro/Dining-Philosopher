#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define HUNGRY 0
#define EATING 1
#define THINKING 2
int state[5];//to keep tack of the state of each philosopher
pthread_cond_t cond_var[5];//to keep track of each philosopher's condition variables
pthread_mutex_t mutex;//lock to handle access to the monitor
time_t curr;//helps to to get time 

//this function tests if the 2 forks are available before
// philosopher can start eating. 
void pickup_forks(int ph_num)
{
    pthread_mutex_lock(&mutex); //acquires lock 
    state[ph_num] = HUNGRY;
    test(ph_num);
    while (state[ph_num] != EATING)//waits until condition variables becomes true
        pthread_cond_wait(&cond_var[ph_num],&mutex);
    pthread_mutex_unlock(&mutex);//releases lock
}

//This function allows the adjacent philosophers to acquire the lock 
//and enter the monitor. 
void return_forks(int ph_num)
{
    pthread_mutex_lock(&mutex);
    state[ph_num] = THINKING;
    test((ph_num + 4) % 5);
    test((ph_num + 1) % 5);
    pthread_mutex_unlock(&mutex);
}

//if the philosopher is free to eat. They eat. 
void test(int ph_num){
    if ((state[(ph_num + 4) % 5] != EATING) &&(state[ph_num] == HUNGRY) &&(state[(ph_num + 1) % 5] != EATING))
     { 
         state[ph_num] = EATING;
         pthread_cond_signal(&cond_var[ph_num]);
     }
}

//This function randomly decides what action should be taken.
void *philosopher(int param)
{

    int index = param;
    srand(time(0));
    
    int n=0;
    
    while (n<10)
    {
        int s = rand() % 2;
        if (s ==1)
        {
            time(&curr);
            //printf("%ld Philosopher %d is thinking\n",((curr.tv_sec*1000000) + curr.tv_usec)/1000, index);
            printf("%ld Philosopher %d is thinking\n",curr, index);
            sleep(1);
        }
        else
        {
            pickup_forks(index);
            time(&curr);
            //printf("%ld Philosopher %d is eating\n",((curr.tv_sec*1000000) + curr.tv_usec)/1000, index);
            printf("%ld Philosopher %d is eating\n",curr, index);
            sleep(3);
            return_forks(index);
            n++;

        }
    }
    pthread_exit(0);
}



int main(int argc, char *argv[])
{

    
    pthread_t tid_arr[5];       /* the thread identifier */
    pthread_attr_t attr_arr[5]; /* set of thread attributes */

    
    pthread_mutex_init(&mutex, NULL);

    for(int i=0; i<5;i++){
        state[i] = THINKING;
        pthread_cond_init(&cond_var[i], NULL);
    }


    for (int i = 0; i < 5; i++)
    {
        /* get the default attributes */

        pthread_attr_init(&attr_arr[i]);
        
        /* create the thread */
        
        pthread_create(&tid_arr[i], &attr_arr[i], philosopher, i);

    }

    //wait for all threads to complete.
    for (int i = 0; i < 5; i++)
        pthread_join(tid_arr[i], NULL);

    //destroys the mutex lock.
    pthread_mutex_destroy(&mutex);

    return 0;
}
