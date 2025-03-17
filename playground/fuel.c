#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

pthread_mutex_t lock;
int fuel = 0;
void* fuel_filling(void* arg){
    for (int i = 0; i<5; i++){

        pthread_mutex_lock(&lock);
        fuel += 15;
        printf("Filled fuel.... %d\n", fuel);
        pthread_mutex_unlock(&lock);
        sleep(1);

    }


}

void* car (void* arg){

    pthread_mutex_lock(&lock);
    while(fuel < 40){
        printf("No fuel.... waiting\n");
        sleep(1);
    }

    fuel -= 40;

    printf("Got fuel. Now left: %d\n", fuel);
    pthread_mutex_unlock(&lock);


    


}


int main(){

    pthread_t threads[2];

    pthread_mutex_init(&lock, NULL);


    for (int i = 0; i <2; i++){

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

    for(int i = 0; i < 2; i++){
        if(pthread_join(threads[i], NULL)!=0){
            perror("Failed to join threads");
        }
    }

    pthread_mutex_destroy(&lock);
    return 0;
}