#include "containers.h"
#include "header.h"






int create_shared_memory(int MAX_PLANES) {
    
     int shmid = shmget(Plane_key, MAX_PLANES * sizeof(Plane), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    
    return shmid;
}
    

Plane *attach_shared_memory(int shmid) {
    void *shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void *) -1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }
    return (Plane *) shmaddr;
}


void detach_shared_memory(void *shmaddr) {
    if (shmdt(shmaddr) < 0) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }
}

void destroy_shared_memory(int shmid) {
    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        perror("shmctl failed");
        exit(EXIT_FAILURE);
    }
}

//--------------------------------------------------------------------------------------



