#include "containers.h"
#include "header.h"
#include "read.h"
#include "semaphores.h"

Config c;

int worker_id;

void splitting_Behavior();
void execute_distributing_worker_code(int worker_id);
void execute_collecting_worker_code(int worker_id);


int main(int argc, char *argv[]) {

     c = read_config("userdefined.txt");
     //printf("Numbers Of Families : %d \n",c.numFamilies);
     
     
       if (argc != 2) {
    fprintf(stderr, "Usage: %s <family_id>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  worker_id = atoi(argv[1]);
  
  splitting_Behavior();
  
  
  return 0;
  
  
 }
 
 void splitting_Behavior(){
 
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
  
  // Initialize semaphore
    sem_t *container_mutex = get_semaphore(containers_sem_key);

    while (1) {
        // Acquire semaphore before entering the critical section
        lock_sem(container_mutex);

        // Continuously monitor for changes in killed workers
        if (shared_data->num_Killed_collectingWorkers > 0) {
            // Update shared memory to reflect the executed code for collecting worker
            shared_data->num_Killed_collectingWorkers--;
            shared_data->num_collectingWorkers++;
            
            shared_data->num_splittingWorkers--;

            // Release semaphore before executing code for collecting worker
            unlock_sem(container_mutex);

            // Execute code for killed collecting worker
            execute_collecting_worker_code(worker_id);

        } else if (shared_data->num_Killed_distributingWorkers > c.threshold_deadDistributers) {
        
            printf("threshold_deadDistributers = %d\n" , c.threshold_deadDistributers);
            // Update shared memory to reflect the executed code for distributing worker
            shared_data->num_Killed_distributingWorkers--;
            shared_data->num_distributingWorkers++;
            
            shared_data->num_splittingWorkers--;

            // Release semaphore before executing code for distributing worker
            unlock_sem(container_mutex);

            // Execute code for killed distributing worker
            execute_distributing_worker_code(worker_id);
        } else {
            // Release semaphore if no action is taken in this iteration
            unlock_sem(container_mutex);
        }

        // Sleep for some time before checking again
        sleep(1);
    }

    // Close the semaphore after it's completely done being used
    close_semaphore(container_mutex);
}
 
 // Function to execute code for a killed collecting worker
void execute_collecting_worker_code(int worker_id) {
    // Convert worker ID to string
    char worker_id_str[20];
    snprintf(worker_id_str, sizeof(worker_id_str), "%d", worker_id);
    printf("splitting worker will execute collecting worker code\n");
    // Execute the collecting worker executable
    if (execl("./collectingWorker", "collectingWorker", worker_id_str, (char *)NULL) == -1) {
        perror("execl");
        exit(EXIT_FAILURE);
    }
}

 // Function to execute code for a killed distributing worker
void execute_distributing_worker_code(int worker_id) {
    // Convert worker ID to string
    char worker_id_str[20];
    snprintf(worker_id_str, sizeof(worker_id_str), "%d", worker_id);
    printf("splitting worker will execute distributing worker code\n");
    // Execute the collecting worker executable
    if (execl("./distributingWorker", "distributingWorker", worker_id_str, (char *)NULL) == -1) {
        perror("execl");
        exit(EXIT_FAILURE);
    }
}
