#include "containers.h"
#include "read.h"
#include "header.h"
#include "semaphores.h"
#include "local.h"



Config c;


int family_id;
//Family *shared_data = NULL;

int get_starvationRate();
void sendto_distribution(int starvation);
int update_starvation_rate();
void check_starvation();
void signal_handler(int sig);

int main(int argc, char *argv[]) {

     c = read_config("userdefined.txt");
     //printf("Numbers Of Families : %d \n",c.numFamilies);
     
     
       if (argc != 2) {
    fprintf(stderr, "Usage: %s <family_id>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  family_id = atoi(argv[1]);
  
    // Register signal handler for SIGUSR1
  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGUSR1, &sa, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }
  
 
  
 
      // get starvation rate of the family
         int starvation = get_starvationRate();
         
    while (1) {
    
         
         //send starvation rate to distribution team
         sendto_distribution(starvation);
         
         
        // Sleep for some time before the next update
        sleep(60); // Sleep for 30 seconds before the next update
         
        //check if starvation rate is greater than threshold_starveToDie, the family dies
        check_starvation();
        
        // Update starvation rates periodically
        starvation = update_starvation_rate( );

    }

    
    return 0;
}


int get_starvationRate(){

    // Seed the random number generator with the process ID
    srand(getpid());

   //The family will have a random starvation rate at the beggining
   int starvation = rand() % (c.max_starvation - c.min_starvation + 1) + c.min_starvation;
   
   // assign the starvation rate in shared memory
       //attach to the shared memory to assign the starvation rate
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
  
  shared_data[family_id].id = family_id;
  shared_data[family_id].starvationRate = starvation;
  shared_data[family_id].died = 0;
 
   detach_shared_memory(shared_data);
  
  return starvation;
  
}

void sendto_distribution(int starvation_rate){

  int mid, n;
  MESSAGE msg;

    //access message queue (Q1) to send the starvation rate to the distribution team
        if ((mid = msgget(Q1_KEY, 0)) == -1) {
        mid = msgget(Q1_KEY, IPC_CREAT | 0660);
    }
    
      // Send starvation rate to distribution team
    msg.msg_to = DISTRIBUTION_TEAM_TYPE;
    msg.msg_fm = getpid();
    *((int *)msg.buffer) = starvation_rate; // Store starvation rate in the buffer
    n = sizeof(msg.msg_to) + sizeof(msg.msg_fm) + sizeof(int); // Set message size
    
    printf("sent: %d\n", *((int *)msg.buffer));

    if (msgsnd(mid, &msg, n, 0) == -1) {
        perror("Client: msgsnd");
        return 2;
    }

    printf("Starvation rate %d from family %d is sent to distribution team.\n",starvation_rate, family_id );
    

}

// Signal handler function
void signal_handler(int sig) {
   
   printf("signal is sent to family with PID %d\n", getpid());
   
   //the family received a userdefined number of flour bags
   int received_bags = c.bags_eachFamily;
   
       //attach to the shared memory to assign the starvation rate
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
   
   //write the value to shared memory
   shared_data[family_id].numOfBags = received_bags;
   
  //after the family took flour bags, their starvation rate is decreased 
  int starvation = shared_data[family_id].starvationRate;
  starvation -= 10;
  shared_data[family_id].starvationRate =  starvation;
  
  printf("after receiving flour, starvation for family %d is %d\n", getpid(), shared_data[family_id].starvationRate);
  
  detach_shared_memory(shared_data);
    

}

int update_starvation_rate(){

   //attach to the shared memory to assign the starvation rate
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
  
  //the starvation rate increases after 30s
  
  int starvation = shared_data[family_id].starvationRate;
  starvation += 20;
  shared_data[family_id].starvationRate =  starvation;
  
  printf("after 30sec, starvation for family %d is %d\n", getpid(), shared_data[family_id].starvationRate);
  
  detach_shared_memory(shared_data);
  return starvation;
  

}

void check_starvation(){

      //attach to the shared memory to assign the starvation rate
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
  
  int starvation = shared_data[family_id].starvationRate;
  
 
  
  if(starvation >= c.threshold_starveToDie){
    
    shared_data[family_id].died = 1;
  
    fprintf(stderr, "Starvation level exceeds threshold. Terminating process.\n");
     detach_shared_memory(shared_data);
    exit(EXIT_FAILURE);
  
  }
}




