#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define nthreads 5

pthread_mutex_t lock;
pthread_cond_t condFuel;


int fuel = 0;
void* fuel_filling(void* arg){
    for (int i = 0; i<5; i++){

        pthread_mutex_lock(&lock);
        fuel += 150;
        printf("Filled fuel.... %d\n", fuel);
        pthread_mutex_unlock(&lock);
        pthread_cond_signal(&condFuel);
        sleep(1);

    }


}

void* car (void* arg){

    pthread_mutex_lock(&lock);
    while(fuel < 40){
        printf("No fuel.... waiting\n");
        pthread_cond_wait(&condFuel, &lock);
        /*Basically it's like:
            
            pthread_mutex_unlock(&lock)
            wait for signal on condFuel
            pthread_mutex_lock(&mutexFuel);

        */
    }

    fuel -= 40;

    printf("Got fuel. Now left: %d\n", fuel);
    pthread_mutex_unlock(&lock);





}


int main(){

    pthread_t threads[nthreads];

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&condFuel, NULL);


    for (int i = 0; i <nthreads; i++){

        if (i==1){
            if(pthread_create(&threads[i], NULL, &fuel_filling, NULL)!=0){
                perror("failed to create thread");
            }
        }

        else{
            if(pthread_create(&threads[i], NULL, &car, NULL)!= 0){
                perror("failed to create thread");
            }
        }
    }

    for(int i = 0; i < nthreads; i++){
        if(pthread_join(threads[i], NULL)!=0){
            perror("Failed to join threads");
        }
    }

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&condFuel);
    return 0;
}