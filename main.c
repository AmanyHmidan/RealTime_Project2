#include "containers.h"
#include "header.h"
#include "read.h"
#include "semaphores.h"



void create_all_semaphores();
void create_messageQueues();
void create_sharedMemories();


void create_ShM1();
void create_ShM2();
void create_semaphore_ShM2();
void create_semaphore_ShM3();
void create_semaphore_ShM6();
void create_semaphore_Q2();
void create_ShM3();
void create_ShM4();
void create_ShM5();
void create_ShM6();


void create_Q1();
void create_Q2();

void clean_all_shared_memories();

void clean_ShM1();
void clean_ShM2();
void clean_ShM3();
void clean_ShM4(); 
void clean_Plane();

void clean_all_semaphores();
void clean_semaphore_ShM2();
void clean_semaphore_ShM3();
void clean_semaphore_Q2();

void clean_all_message_queues();
void clean_Q1();
void clean_Q2();

int simulation_end_condition_met();
int Killed_CollectingWorkers__exceededThreshold();
int Killed_DistributingWorkers__exceededThreshold();
int number_of_families_starved_exceeded_threshold();
int running_time_exceeded_threshold(double threshold_seconds);
int number_of_wheat_flour_containers_shot_down_exceeded_threshold();

Config c;
#define MAX_CHILDREN 50
 pid_t drawer;


int child_pids[MAX_CHILDREN];
int num_children = 0;

void terminate_children() {
    printf("Time limit reached. Terminating all child processes...\n");
    for (int i = 0; i < num_children; i++) {
        kill(child_pids[i], SIGTERM);  // Send SIGTERM to each child
        printf("Sent SIGTERM to child PID %d\n", child_pids[i]);
    }
    kill(drawer, SIGTERM);
    
}

// Global variable to store the start time of the simulation
time_t start_time;

// Function to initialize the start time of the simulation
void initialize_start_time() {
    time(&start_time);
}

// Function to calculate the elapsed time since the start of the simulation in seconds
double elapsed_time() {
    time_t current_time;
    time(&current_time);
    return difftime(current_time, start_time);
}

int main() {

  // Initialize start time
    initialize_start_time();

  c = read_config("userdefined.txt");

printf("num_splittingWorkers = %d\n", c.num_splittingWorkers);
printf("num_distributingWorkers = %d\n", c.num_distributingWorkers);
printf("threshold_starveToDie = %d\n", c.threshold_starveToDie);
printf("num_collectingTeams = %d\n", c.num_collectingTeams);
printf("num_cargoPlanes = %d\n", c.num_cargoPlanes);
printf("min_containersNum = %d\n", c.min_containersNum);
printf("max_containersNum = %d\n", c.max_containersNum);
printf("containersWeight = %d\n", c.containersWeight);
printf("min_containerPeriod = %d\n", c.min_containerPeriod);
printf("max_containerPeriod = %d\n", c.max_containerPeriod);
printf("numFamilies = %d\n", c.numFamilies);
printf("max_starvation = %d\n", c.max_starvation);
printf("min_starvation = %d\n", c.min_starvation);
printf("bags_eachWorker = %d\n", c.bags_eachWorker);
printf("min_containerRefill = %d\n", c.min_containerRefill);
printf("max_containerRefill = %d\n", c.max_containerRefill);
printf("ALTITUDE_THRESHOLD = %d\n", c.ALTITUDE_THRESHOLD);
printf("initial_ALTITUDE = %d\n", c.initial_ALTITUDE);
printf("num_collectingWorkers = %d\n", c.num_collectingWorkers);
printf("max_energy = %d\n", c.max_energy);
printf("min_energy = %d\n", c.min_energy);
printf("bags_eachFamily = %d\n", c.bags_eachFamily);
printf("endGame_deadCollectors = %d\n", c.endGame_deadCollectors);
printf("endGame_deadDistributers = %d\n", c.endGame_deadDistributers);
printf("endGame_deadFamilies = %d\n", c.endGame_deadFamilies);
printf("threshold_deadDistributers = %d\n", c.threshold_deadDistributers);
printf("endGame_hitContainers = %d\n", c.endGame_hitContainers);
printf("Terminate_time = %lf\n", c.Terminate_time);


  // create Planes shared memory
  int shmid = create_shared_memory(c.num_cargoPlanes);
  Plane *planes = attach_shared_memory(shmid);
  // planes is a pointer to the beginning of the shared memory segment, Each
  // element of the planes array holds information about a specific plane.
  
  
  create_sharedMemories();
  
  create_all_semaphores();
  
  create_messageQueues();
  
  
 // pid_t drawer;

    if((drawer = fork()) == -1){
        perror("The drawer fork error\n");
        exit(-1);
    }

    // To let the drawer leave the main code and go to drawer.c
    if(!drawer){
        execlp("./Drawer", "drawer", (char *) NULL);

        // If the program not have enough memory then will raise error
        perror("exec Failure\n");
        exit(-1);
    }
  sleep(1);


  // fork the planes
  for (int i = 0; i < c.num_cargoPlanes; i++) {
    pid_t pid = fork();
    if (pid == 0) {
      planes[i + 1].planeID = i + 1; // Set plane ID in shared memory
      char plane_id_str[10];
      sprintf(plane_id_str, "%d", i + 1);
      execl("./cargo_plane", "cargo_plane", plane_id_str, (char *)NULL);
      perror("execl failed");
      exit(EXIT_FAILURE);
    } else if (pid < 0) {
      perror("fork failed");
      exit(EXIT_FAILURE);
    }else if (pid > 0) {  // Parent process
            child_pids[num_children++] = pid;
        } 
  }

  sleep(2);
  
  //fork occupation forces
     pid_t pid = fork();
    if (pid == 0) {
      
      execl("./occupationForces", "occupationForces", (char *)NULL);
      perror("execl failed");
      exit(EXIT_FAILURE);
    } else if (pid < 0) {
      perror("fork failed");
      exit(EXIT_FAILURE);
    }else if (pid > 0) {  // Parent process
            child_pids[num_children++] = pid;
        } 
  
  
  sleep(2);

  // fork the collecting teams
  for (int i = 0; i < c.num_collectingTeams; i++) {
    pid_t pid = fork();
    if (pid == 0) {
      char team_id_str[10];
      sprintf(team_id_str, "%d", i + 1);
      execl("./collectingTeam", "collectingTeam", team_id_str, (char *)NULL);
      perror("execl failed");
      exit(EXIT_FAILURE);
    } else if (pid < 0) {
      perror("fork failed");
      exit(EXIT_FAILURE);
    }else if (pid > 0) {  // Parent process
            child_pids[num_children++] = pid;
        } 
  }
  
   sleep(1);
  //fork collecting workers
    for (int i = 0; i < c.num_collectingWorkers; i++) {
    pid_t pid = fork();
    if (pid == 0) {
      char worker_id_str[10];
      sprintf(worker_id_str, "%d", i );
      execl("./collectingWorker", "collectingWorker", worker_id_str, (char *)NULL);
      perror("execl failed");
      exit(EXIT_FAILURE);
    } else if (pid < 0) {
      perror("fork failed");
      exit(EXIT_FAILURE);
    }else if (pid > 0) {  // Parent process
            child_pids[num_children++] = pid;
        } 
  }

  sleep(10);
  // fork the splitting team leader
  for (int i = 0; i < 1; i++) {
    pid_t pid = fork();
    if (pid == 0) {
      char team_id_str[10];
      sprintf(team_id_str, "%d", i + 1);
      execl("./splittingTeam", "splittingTeam", team_id_str, (char *)NULL);
      perror("execl failed");
      exit(EXIT_FAILURE);
    } else if (pid < 0) {
      perror("fork failed");
      exit(EXIT_FAILURE);
    }else if (pid > 0) {  // Parent process
            child_pids[num_children++] = pid;
        } 
  }
  
  sleep(1);
  //fork splitting workers
    for (int i = 0; i < c.num_splittingWorkers; i++) {
    pid_t pid = fork();
    if (pid == 0) {
      char worker_id_str[10];
      sprintf(worker_id_str, "%d", i );
      execl("./splittingWorker", "splittingWorker", worker_id_str, (char *)NULL);
      perror("execl failed");
      exit(EXIT_FAILURE);
    } else if (pid < 0) {
      perror("fork failed");
      exit(EXIT_FAILURE);
    }else if (pid > 0) {  // Parent process
            child_pids[num_children++] = pid;
        } 
  }
  
  
  sleep(10);
  //fork the distributingTeam (leader) 
  for (int i = 0; i < 1; i++) {
    pid_t pid = fork();
    if (pid == 0) {
      char team_id_str[10];
      sprintf(team_id_str, "%d", i + 1);
      execl("./distributingTeam", "distributingTeam", team_id_str, (char *)NULL);
      perror("execl failed");
      exit(EXIT_FAILURE);
    } else if (pid < 0) {
      perror("fork failed");
      exit(EXIT_FAILURE);
    }else if (pid > 0) {  // Parent process
            child_pids[num_children++] = pid;
        } 
  }
  
  sleep(1);
  //fork distributing workers
  int j = c.num_collectingWorkers;
  for (int i = 0; i < c.num_distributingWorkers; i++) {
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 4;
    } else if (pid == 0) { // Child process (worker)
        char worker_id_str[10];
        char number_str[10];
        sprintf(worker_id_str, "%d", i);
        sprintf(number_str, "%d", j);
        execl("./distributingWorker", "distributingWorker", worker_id_str, number_str, (char *)NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // Parent process
        child_pids[num_children++] = pid;
    } 
    j++; 
}

  
  sleep(10);
  //fork the Families
  for (int i = 0; i < 5; i++) {
    pid_t pid = fork();
    if (pid == 0) {
      char family_id_str[10];
      sprintf(family_id_str, "%d", i + 1);
      execl("./family", "family", family_id_str, (char *)NULL);
      perror("execl failed");
      exit(EXIT_FAILURE);
    } else if (pid < 0) {
      perror("fork failed");
      exit(EXIT_FAILURE);
    }else if (pid > 0) {  // Parent process
            child_pids[num_children++] = pid;
        } 
  }
  
  sleep(180);
  
  // End of simulation conditions
    if (simulation_end_condition_met()) {
        printf("Simulation ended.\n");
    } else {
        printf("Simulation ended due to an error.\n");
    }
  
  //sleep (180);
    
    // After sleep, terminate all child processes
  
  printf("clean all\n");

  clean_all_semaphores();
  clean_all_shared_memories();
  clean_all_message_queues();
  //terminate_children();

  
  return 0;
}

//-----------------------------------------------------------------------

// Function to check if any simulation end condition is met
int simulation_end_condition_met() {
    // Check each end condition
    /*if (running_time_exceeded_threshold(180)) {
         terminate_children();
        return 1;
    }*/


    if (number_of_wheat_flour_containers_shot_down_exceeded_threshold()) {
         terminate_children();
        return 1;
    }

    if (Killed_CollectingWorkers__exceededThreshold()) {
         terminate_children();
        return 1;
    }

    if (Killed_DistributingWorkers__exceededThreshold()) {
    
         terminate_children();
        return 1;
    }

    if (number_of_families_starved_exceeded_threshold()) {
        terminate_children();
        return 1;
    }

    return 0;  // No simulation end condition met
}

// Function to check if the running time has exceeded a user-defined threshold
int running_time_exceeded_threshold(double threshold_seconds) {
    double elapsed_seconds = elapsed_time();
    if (elapsed_seconds > threshold_seconds) {
        return 1; // Exceeded threshold
    }
    return 0; // Not exceeded threshold
}



int Killed_CollectingWorkers__exceededThreshold(){
   
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
  
  //check if number of killed collecting workers exceeds a user defined threshold
  
  if((shared_data->total_C_killed) > c.endGame_deadCollectors ) {
     printf("Ending simulation : number of killed collecting workers exceeds a user defined threshold\n");
      return 1;
     
  }
  
  return 0; 


}


int Killed_DistributingWorkers__exceededThreshold(){
   
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
  
  //check if number of killed collecting workers exceeds a user defined threshold
  
  if((shared_data->total_D_killed) > c.endGame_deadDistributers ) {
     printf("Ending simulation : number of killed distributing workers exceeds a user defined threshold\n");
      return 1;
     
  }
  
  return 0; 


}

int number_of_families_starved_exceeded_threshold(){
  
  int total = 0;
  int shmid = shmget(family_key, sizeof(Family), 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  Family *shared_data = (Family *)shmat(shmid, NULL, 0);
  if (shared_data == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }
  
  for(int i = 1; i<= c.numFamilies; i++){
     
     if(shared_data[i].died == 1){
        total += 1 ;
     
     }
  
  }
  
  if(total < c.endGame_deadFamilies){
     printf("Ending simulation : number of dead families exceeds a user defined threshold\n");
     return 1;
   }
   
   return 0;


}


int number_of_wheat_flour_containers_shot_down_exceeded_threshold(){
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
  
  detach_shared_memory(shared_data);
  
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
  
  int numberOfHit = 0;
  
  for(int i = 0; i<total; i++){
    
    if(containers[i].hit == 1){
       numberOfHit += 1;
    }
  }

  if(numberOfHit > c.endGame_hitContainers){
   printf(" number_of_wheat_flour_containers_shot_down_exceeded_threshold\n");
     return 1;
     
   }
   
   return 0;


}



//------------------------------------------------------------------------

void create_sharedMemories(){

    // create total containers shared memory
  create_ShM1();

  // create totalcontainers shared memory
  create_ShM2();

  // create shared memory for storage
  create_ShM3();
  
  //create shared memory for families 
  create_ShM4();
  
   //create shared memory for Workers
  create_ShM5();
  
   //create shared memory for Number of workers
  create_ShM6();
  
  


}

void create_all_semaphores(){

     
   // create semaphore for ShM2 (totalcontainers)
  create_semaphore_ShM2();
  
    // create semaphore for ShM3 (Storage)
  create_semaphore_ShM3();
  
    // create semaphore for Q2
  create_semaphore_Q2();
  
  //create semaphore for SHM 6 (number of workers )
  create_semaphore_ShM6();


}

void create_messageQueues(){
 //Q1 for communication between Families and ditribution team leader
   create_Q1();
 //Q1 for communication between ditribution team leader and workers
   create_Q2();
   
}


void create_Q2(){
   
     if ( msgget(Q2_KEY, IPC_CREAT | 0777) == -1 ) {
        perror("Q2 Create Error\n");
         exit(EXIT_FAILURE);
    }
}
void create_Q1(){
   
     if ( msgget(Q1_KEY, IPC_CREAT | 0777) == -1 ) {
        perror("Q1 Create Error\n");
         exit(EXIT_FAILURE);
    }
}


void clean_all_semaphores() { 
clean_semaphore_ShM2();
clean_semaphore_ShM3() ;
clean_semaphore_Q2() ; 
}



void clean_all_shared_memories() {

  clean_ShM1();
  clean_ShM2();
  clean_ShM3();
  clean_ShM4();
  clean_Plane();
}

void clean_ShM1() {
  int shmid = shmget(totalContainers_key, sizeof(totalContainers), 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    perror("shmctl");
    exit(EXIT_FAILURE);
  }
}
void clean_ShM2() {
  int shmid = shmget(Container_key, sizeof(Container), 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    perror("shmctl");
    exit(EXIT_FAILURE);
  }
}

void clean_ShM3() {
  int shmid = shmget(storage_key, sizeof(Storage), 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    perror("shmctl");
    exit(EXIT_FAILURE);
  }
}
void clean_ShM4() {
  int shmid = shmget(family_key, sizeof(Family), 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    perror("shmctl");
    exit(EXIT_FAILURE);
  }
}
void clean_Plane(){
int shmid = shmget(Plane_key, sizeof(Plane), 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    perror("shmctl");
    exit(EXIT_FAILURE);
  }
}

void clean_all_message_queues(){
    clean_Q1();
    clean_Q2();

}

void clean_Q1(){
    int queue;
    if ( (queue = msgget(Q1_KEY, IPC_CREAT | 0777)) == -1 ) {
        perror("OQ Get Error\n");
        exit(-1);
    }
    msgctl(queue, IPC_RMID, 0);/* remove */
}

void clean_Q2(){
    int queue;
    if ( (queue = msgget(Q2_KEY, IPC_CREAT | 0777)) == -1 ) {
        perror("OQ Get Error\n");
        exit(-1);
    }
    msgctl(queue, IPC_RMID, 0);/* remove */
}

void create_ShM1() {
  int shmid =
      shmget(totalContainers_key, sizeof(totalContainers), IPC_CREAT | 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  totalContainers *shared_data = (totalContainers *)shmat(shmid, NULL, 0);
  if (shared_data == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }

  shared_data->totalContainers = 0;

  if (shmdt(shared_data) == -1) {
    perror("shmdt");
    exit(EXIT_FAILURE);
  }
}

void create_ShM2() {
  int shmid = shmget(Container_key, sizeof(Container), IPC_CREAT | 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  Container *shared_data = (Container *)shmat(shmid, NULL, 0);
  if (shared_data == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }

  if (shmdt(shared_data) == -1) {
    perror("shmdt");
    exit(EXIT_FAILURE);
  }
}

void create_ShM3() {

  
  int shmid = shmget(storage_key, sizeof(Storage), IPC_CREAT | 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  Storage *shared_data = (Storage *)shmat(shmid, NULL, 0);
  if (shared_data == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }

  shared_data->flourStorage = 0;
  shared_data->bags_number = 0;
  for(int i = 0; i < c.num_distributingWorkers; i++){
    shared_data->bags_worker[i] = 0;
  
  }

  if (shmdt(shared_data) == -1) {
    perror("shmdt");
    exit(EXIT_FAILURE);
  }
}

void create_ShM4() {
    int shmid = shmget(family_key, sizeof(Family), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    Family *shared_data = (Family *)shmat(shmid, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    
    shared_data->starvationRate = 0;
    shared_data->numOfBags = 0;

    if (shmdt(shared_data) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}

void create_ShM5() {
    int shmid = shmget(Workers_KEY, sizeof(Workers), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    Workers *shared_data = (Workers *)shmat(shmid, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    
    
    if (shmdt(shared_data) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}

void create_ShM6() {

    
  
    int shmid = shmget(Number_of_Workers_KEY, sizeof(Number_of_Workers), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    Number_of_Workers *shared_data = (Number_of_Workers *)shmat(shmid, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    
   shared_data->num_collectingWorkers = c.num_collectingWorkers;
   shared_data->num_splittingWorkers = c.num_splittingWorkers;
   shared_data->num_distributingWorkers = c.num_distributingWorkers;
 
   shared_data->num_Killed_collectingWorkers = 0;
   shared_data->num_Killed_distributingWorkers = 0;
    
  

    if (shmdt(shared_data) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}

void create_semaphore_ShM2() {

  sem_t *cnm_sem = sem_open(containers_sem_key, O_CREAT, 0777, 0);
  if (cnm_sem == SEM_FAILED) {
    perror("CNM Semaphore Open Error\n");
    exit(-1);
  }

  // When return -1 then wrong issue happened
  if (sem_post(cnm_sem) < 0) {
    perror("CNM Semaphore Release Error\n");
    exit(-1);
  }
  sem_close(cnm_sem);
}

void create_semaphore_ShM3() {

  sem_t *cnm_sem = sem_open(storage_sem_key, O_CREAT, 0777, 0);
  if (cnm_sem == SEM_FAILED) {
    perror("CNM Semaphore Open Error\n");
    exit(-1);
  }

  // When return -1 then wrong issue happened
  if (sem_post(cnm_sem) < 0) {
    perror("CNM Semaphore Release Error\n");
    exit(-1);
  }
  sem_close(cnm_sem);
}

void create_semaphore_ShM6() {

  sem_t *cnm_sem = sem_open(SHM6_sem_key, O_CREAT, 0777, 0);
  if (cnm_sem == SEM_FAILED) {
    perror("CNM Semaphore Open Error\n");
    exit(-1);
  }

  // When return -1 then wrong issue happened
  if (sem_post(cnm_sem) < 0) {
    perror("CNM Semaphore Release Error\n");
    exit(-1);
  }
  sem_close(cnm_sem);
}

void create_semaphore_Q2(){

   sem_t *cnm_sem = sem_open(Q2_sem_key, O_CREAT, 0777, 0);
  if (cnm_sem == SEM_FAILED) {
    perror("CNM Semaphore Open Error\n");
    exit(-1);
  }

  // When return -1 then wrong issue happened
  if (sem_post(cnm_sem) < 0) {
    perror("CNM Semaphore Release Error\n");
    exit(-1);
  }
  sem_close(cnm_sem);


}

void clean_semaphore_ShM2() {
  if (sem_unlink(containers_sem_key) < 0) {
    perror("STM Unlink Error\n");
    exit(-1);
  }
}
void clean_semaphore_ShM3() {
  if (sem_unlink(storage_sem_key) < 0) {
    perror("STM Unlink Error\n");
    exit(-1);
  }
}
void clean_semaphore_Q2() {
  if (sem_unlink(Q2_sem_key) < 0) {
    perror("STM Unlink Error\n");
    exit(-1);
  }
}






