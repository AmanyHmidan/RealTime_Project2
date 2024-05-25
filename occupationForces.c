#include "containers.h"
#include "header.h"
#include "read.h"
#include "semaphores.h"
#include "local.h"
#include <math.h>

Config c;

void kill_workers_behaviour();
void update_workersNum(int team);
void detach_shared_memory(void *shmaddr);
void occupationForces_behaviour();

int main(int argc, char *argv[]) {
    // Your worker code here
     
     srand(time(NULL));
     
      c = read_config("userdefined.txt");
      
     
      occupationForces_behaviour();
      sleep(50);
      kill_workers_behaviour();
      
      while(1){
      sleep(40);
      occupationForces_behaviour();
      kill_workers_behaviour();
      
      }
      
      
      
      return 0;
      
}

void kill_workers_behaviour(){
    
 //get the total number of workers 
  int shmid = shmget(Number_of_Workers_KEY, sizeof(Number_of_Workers), 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  Number_of_Workers *shared_data = (Number_of_Workers *)shmat(shmid, NULL, 0);
  if (shared_data == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }
  
  int NUM_WORKERS = shared_data->num_distributingWorkers  +  shared_data->num_collectingWorkers;
  
  detach_shared_memory(shared_data); 
  
  int shmid2 = shmget(Workers_KEY, sizeof(Workers), 0666);
  if (shmid2 == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  Workers *workers = (Workers *)shmat(shmid2, NULL, 0);
  if (shared_data == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }
  
  //iterate through workers and try to kill them depending on their energy level
   for (int i = 0; i < NUM_WORKERS; i++) {
   
        if(workers[i].killed == 0){
        double probability = 1.0 - ((double)workers[i].energy / 100.0);
        int random_number = rand() % 100; // Generating random number between 0 and 99
        
        int team = workers[i].team;
        char *Team; // Pointer to characters
        if(team == 0){
          Team = "collecting" ;
        
        }else if(team == 1){
           Team = "distributing" ;
        }

        if (random_number < probability * 100) {
            workers[i].killed = 1; // Worker is killed
            printf("%s Worker %d with PID %d is killed\n", Team, i , workers[i].pid);
            
            update_workersNum(team);
            
             // Send SIGKILL signal to kill the worker
            if (kill(workers[i].pid, SIGKILL) == -1) {
                fprintf(stderr, "Error killing worker %d: %s\n", i , strerror(errno));
                
                
            }
        }
        
       }
    }
    
     detach_shared_memory(workers); 
  

}


void update_workersNum(int team){
  
    int shmid = shmget(Number_of_Workers_KEY, sizeof(Number_of_Workers), 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  Number_of_Workers *shared_data = (Number_of_Workers *)shmat(shmid, NULL, 0);
  if (shared_data == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }
  
  if(team == 0){
    shared_data->num_collectingWorkers -= 1;
    shared_data->num_Killed_collectingWorkers += 1;
    shared_data->total_C_killed += 1;
    printf("total Killed collectingWorkers = %d\n", shared_data->total_C_killed);
  }else if(team ==1){
    shared_data->num_distributingWorkers -=1;
    shared_data->num_Killed_distributingWorkers += 1;
    shared_data->total_D_killed += 1;
    printf("total Killed distributingWorkers = %d\n", shared_data->total_D_killed);
  }
  
  
  
  detach_shared_memory(shared_data); 


}

 void detach_shared_memory(void *shmaddr) {
    if (shmdt(shmaddr) < 0) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }
}


void occupationForces_behaviour() {

  // take the total number of containers
  int shmid = shmget(totalContainers_key, sizeof(totalContainers), 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  totalContainers *shared_data = (totalContainers *)shmat(shmid, NULL, 0);
  if (shared_data == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }

  int total = shared_data->totalContainers;

  

  // open the shared memory of containers to see which containers are still in
  // the air
  int shmid2 = shmget(Container_key, sizeof(Container), 0666);
  if (shmid2 == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  Container *containers = (Container *)shmat(shmid2, NULL, 0);
  if (containers == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }
  printf("befor shooooot\n");

  // Choose a random container that is still in the air and shoot it
  int randomIndex;
  do {
    randomIndex = rand() % shared_data->totalContainers;
  } while (containers[randomIndex].dropped != 0);
  
      
	sleep(1);
  printf("Occupation forces shoot container %d at height %d meters\n",
         containers[randomIndex].containerID, containers[randomIndex].height);

  //  handle the shooting of the container
  int id = containers[randomIndex].containerID;
  containers[id].hit = 1;

  if (containers[id].height > c.ALTITUDE_THRESHOLD) {
    printf("Container %d completely destroyed\n", id);
    containers[id].content = 0; // Completely destroyed
    containers[id].destroyed = 1;
    containers[id].dropped = 0;
    sleep(5);
  } else {

    // Calculating the percentage of content salvaged based on the height
    double salvagePercentage = fmax(0, 100 - 0.1 * containers[randomIndex].height);
    printf("Percentage of content salvaged: %.2f%%\n", salvagePercentage);

    containers[id].content =  containers[id].content * (salvagePercentage / 100.0);
        
    printf("New content salvaged: %.2f\n", containers[id].content);
    // Mark container as dropped
  containers[randomIndex].dropped = 1;
    sleep(5);
  }
  

  
  //sleep(5);

  detach_shared_memory(containers);
  detach_shared_memory(shared_data);
}
