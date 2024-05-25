#include "containers.h"
#include "header.h"
#include "read.h"
#include "semaphores.h"
#include "local.h"

int Dworker_id;
int number;

Config c;

void workerBehavior();
void detach_shared_memory(void *shmaddr);
void receive_familyPID();
void sendFlouerToFamily(int family_pid);
void check_haveFlour();
void my_energyLevel();
void decrease_energy();

int main(int argc, char *argv[]) {
    // Your worker code here
    
      c = read_config("userdefined.txt");
    
     if (argc != 3) {
        fprintf(stderr, "Usage: %s <worker_id_str> <number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    Dworker_id = atoi(argv[1]);
    number = atoi(argv[2]);
    
     // Get the current process ID
    pid_t pid = getpid();
    
    // Seed the random number generator with the process ID
    srand((unsigned int)pid);

    
  //take an energy level
  my_energyLevel();
    
    workerBehavior();
    
   
    return 0;
}

void workerBehavior(){

 int shmid2 = shmget(storage_key, sizeof(Storage), 0666);
  if (shmid2 == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  Storage *storage = (Storage *)shmat(shmid2, NULL, 0);
  if (storage == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }
  
  sleep(10);
  
    // get semaphore, lock shared memory to update this container's value to be
  // TAKEN.
  sem_t *storage_mutex = get_semaphore(storage_sem_key);

  lock_sem(storage_mutex);
  
  
  //printf("total storage bags = %d, bags_eachWorker = %d\n",storage->bags_number, c.bags_eachWorker);
  if(storage->bags_number > c.bags_eachWorker){
    
    //give a user defined number of bages to the worker
    storage->bags_worker[Dworker_id] = c.bags_eachWorker;
    storage->bags_number -= c.bags_eachWorker;
  
  }else if ( storage->bags_number !=0 ) {
    
    //give the rest bags to the worker
    storage->bags_worker[Dworker_id] = storage->bags_number;
    storage->bags_number -= storage->bags_number;
  
  }
  
  unlock_sem(storage_mutex);
  
  // Close the semaphore here, after it's completely done being used
  close_semaphore(storage_mutex);
  
  printf("worker %d took %d bags,total storage bags = %d\n", Dworker_id, storage->bags_worker[Dworker_id],storage->bags_number);
  
 
  detach_shared_memory(storage);
  
  receive_familyPID();


}

void receive_familyPID(){
  
  //first read from message queue to get a family pid sent from distribution team leader
      int q2_id;

    // Access message queue (Q2)
    if ((q2_id = msgget(Q2_KEY, 0)) == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    // Infinite loop to continuously read messages from the queue
    while (1) {
        MESSAGE msg;
        
          sem_t *storage_mutex = get_semaphore(Q2_sem_key);

          lock_sem(storage_mutex);

        // Receive message from the queue
        if (msgrcv(q2_id, &msg, sizeof(msg), TO_WORKER_TYPE, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
       

        // Process the received message
        int family_pid = atoi(msg.buffer); // Convert buffer to integer
        printf("Received family PID: %d from worker %d\n", family_pid, Dworker_id );
        
        unlock_sem(storage_mutex);
  
       // Close the semaphore here, after it's completely done being used
         close_semaphore(storage_mutex);
         
         //first check if the worker still have flour bags
         check_haveFlour();

        // Send food to the family with the received PID
        sendFlouerToFamily(family_pid);
        
        //after sending food the worker gets tired and his energy level decreaes
        decrease_energy();
    }



}

void sendFlouerToFamily(int family_pid){
  
  printf("sending flour to family with PID %d will start\n", family_pid);
  
  //send signal to notify the family that bags have been send to them
    if (kill(family_pid, SIGUSR1) == -1) {
    perror("kill");
    exit(EXIT_FAILURE);
  }
  
  //decrease bags for worker 
   int shmid2 = shmget(storage_key, sizeof(Storage), 0666);
  if (shmid2 == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  Storage *storage = (Storage *)shmat(shmid2, NULL, 0);
  if (storage == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }
  
  sem_t *storage_mutex = get_semaphore(storage_sem_key);

  lock_sem(storage_mutex);
  
   storage->bags_worker[Dworker_id] -= c.bags_eachWorker;
  
   
    unlock_sem(storage_mutex);
  
    // Close the semaphore here, after it's completely done being used
     close_semaphore(storage_mutex);
     
     detach_shared_memory(storage);
  

}

void check_haveFlour(){

    int shmid2 = shmget(storage_key, sizeof(Storage), 0666);
  if (shmid2 == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  Storage *storage = (Storage *)shmat(shmid2, NULL, 0);
  if (storage == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }
  
  
    // get semaphore, lock shared memory to update this container's value to be
  // TAKEN.
  sem_t *storage_mutex = get_semaphore(storage_sem_key);

  lock_sem(storage_mutex);
  
  
  //if bags for worker = zero, take bags from the storage if available, 
  if(storage->bags_worker[Dworker_id] == 0){
  
  
    while(storage->bags_number == 0 ){
       
       //if no bags are available in the storage, wait until bags are available 
    
    }
  
    if(storage->bags_number > c.bags_eachWorker){
    
    //give a user defined number of bages to the worker
    storage->bags_worker[Dworker_id] = c.bags_eachWorker;
    storage->bags_number -= c.bags_eachWorker;
  
  }else if ( storage->bags_number !=0 ) {
    
    //give the rest bags to the worker
    storage->bags_worker[Dworker_id] = storage->bags_number;
    storage->bags_number -= storage->bags_number;
  
  }
  
  }
  
    unlock_sem(storage_mutex);
  
    // Close the semaphore here, after it's completely done being used
     close_semaphore(storage_mutex);
     
     detach_shared_memory(storage);




}
 void my_energyLevel(){
 
   int energy = rand() % (c.max_energy - c.min_energy + 1) + c.min_energy;
  
   
 //attach to the shared memory of workers to assign the energy rate
  int shmid = shmget(Workers_KEY, sizeof(Workers), 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  Workers *shared_data = (Workers *)shmat(shmid, NULL, 0);
  if (shared_data == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }
  
  shared_data[number].energy = energy;
  shared_data[number].team = 1;
  shared_data[number].pid = getpid();
  shared_data[number].killed = 0;
  
  printf("energy for distributing worker %d number %d is %d\n", Dworker_id, number, shared_data[Dworker_id].energy );
   
    detach_shared_memory(shared_data);
  
  
 
 }
 
 void decrease_energy(){
 
     //attach to the shared memory of workers to assign the energy rate
  int shmid = shmget(Workers_KEY, sizeof(Workers), 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  Workers *shared_data = (Workers *)shmat(shmid, NULL, 0);
  if (shared_data == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }
  
  shared_data[number].energy -= 10;
  
   printf("decreasing energy for distributing worker %d number %d is %d\n", Dworker_id, number, shared_data[Dworker_id].energy );
   
    detach_shared_memory(shared_data);
   
 
 }
 
 void detach_shared_memory(void *shmaddr) {
    if (shmdt(shmaddr) < 0) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }
}


