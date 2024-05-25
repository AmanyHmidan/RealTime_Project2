#ifndef SEMAPHORES
#define SEMAPHORES

#include "header.h"

const char *containers_sem_key = "containers22";
const char *storage_sem_key = "storage44";
const char *Q2_sem_key = "Q2pass33";
const char *SHM6_sem_key = "workersNUMBER55";
/*
 * Sem: 1
 * P1: Sem --> 0 through operation, when finish release it to 1 Sem --> 1
 */
sem_t* get_semaphore(const char* key){

    sem_t* sem = sem_open(key, O_CREAT, 0777, 0);
    if(sem == SEM_FAILED){
        perror("Semaphore Open Error\n");
        exit(-1);
    }

    return sem;
}

void lock_sem(sem_t* sem){

    // When return 0 for successful access
    if (sem_wait(sem) < 0){
        perror("Semaphore Wait Error\n");
        exit(-1);
    }

}

void unlock_sem(sem_t* sem){

    // When return -1 then wrong issue happened
    if (sem_post(sem) < 0){
        perror("Semaphore Release Error\n");
        exit(-1);
    }



}


void close_semaphore(sem_t* sem){
    // Close the Semaphore
    sem_close(sem);
}




#endif
