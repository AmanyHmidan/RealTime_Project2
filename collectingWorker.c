#include "containers.h"
#include "header.h"
#include "read.h"
#include "semaphores.h"

int worker_id;

void my_energyLevel();
void detach_shared_memory(void *shmaddr);
void decrease_energy();

Config c;

int main(int argc, char *argv[]) {

     c = read_config("userdefined.txt");
     //printf("Numbers Of Families : %d \n",c.numFamilies);
     
     
       if (argc != 2) {
    fprintf(stderr, "Usage: %s <family_id>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  worker_id = atoi(argv[1]);
  
   // Get the current process ID
    pid_t pid = getpid();
    
    // Seed the random number generator with the process ID
    srand((unsigned int)pid);

  
  //take an energy level
  my_energyLevel();
  
  while(1){
  //sleep 30sec until collecting new container
  sleep(30);
  
  //worker gets tired after collecting so decrease its energy
  decrease_energy();
  
  }
  
  
  
  return 0;
  
  
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
  
  shared_data[worker_id].energy = energy;
  shared_data[worker_id].team = 0;
  shared_data[worker_id].pid = getpid();
  shared_data[worker_id].killed = 0;
  
   printf("energy for collecting worker %d is %d\n", worker_id, shared_data[worker_id].energy );
   
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
  
  shared_data[worker_id ].energy -= 10;
  
   printf("decreasing energy for collecting worker %d is %d\n", worker_id, shared_data[worker_id].energy );
   
    detach_shared_memory(shared_data);
   
 
 }
 
 void detach_shared_memory(void *shmaddr) {
    if (shmdt(shmaddr) < 0) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }
}
