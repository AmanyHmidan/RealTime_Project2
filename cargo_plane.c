#include "containers.h"
#include "header.h"
#include "read.h"

Config c;
int plane_id;

void cargo_plane_behavior(int id);
void set_containersInfo(int numContainers);
void leave_and_refill();

int main(int argc, char *argv[]) {

  c = read_config("userdefined.txt");

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <plane_id>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  plane_id = atoi(argv[1]);

  cargo_plane_behavior(plane_id);

  return 0;
}

void init_random_seed() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  srand((unsigned int)(ts.tv_nsec ^ getpid()));
}

int get_random_containers() {
  int result = (rand() % (c.max_containersNum - c.min_containersNum + 1)) +
               c.min_containersNum;
  return result;
}

void cargo_plane_behavior(int id) {

  // attach to the shared memory
  int shmid = create_shared_memory(c.num_cargoPlanes);
  Plane *planes = attach_shared_memory(shmid);

  init_random_seed();

  printf("Cargo Plane %d started\n", id);
  int containers = get_random_containers();

  printf("Cargo Plane %d has %d containers\n", id, containers);
  planes[plane_id].numContainers = containers;

  printf("from shared mem: planeID = %d, numContainers = "
         "%d\n--------------------\n",
         planes[plane_id].planeID, planes[plane_id].numContainers);

  // Set containers info
  set_containersInfo(containers);
  
   // Leave airspace and go for re-fill
  leave_and_refill();
}

void set_containersInfo(int Containernum) {

  // update total number of containers and set las container id
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

  shared_data->lastContainer_id = shared_data->totalContainers;
  shared_data->totalContainers += Containernum;

  int k = shared_data->lastContainer_id + 1;
  int total = shared_data->totalContainers;

  detach_shared_memory(shared_data);

  // attach to shared memory of containers to set their values
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

  printf("k= %d, total = %d\n", k, total);

  // setting initial values for containers
  for (int j = k; j <= total; j++) {

    containers[j].containerID = j;
    containers[j].content = c.containersWeight;
    containers[j].taken = 0;
    containers[j].height = c.initial_ALTITUDE;
    //containers[j].height = 600 ;
    containers[j].dropped = 0;
    containers[j].hit = 0;
    containers[j].destroyed = 0;
  }

  // droping the containers
  
  
 // Decrease the height of each container gradually until it reaches 0
    for (int j = k; j <= total; j++) {
        int dropTime = c.min_containerPeriod + rand() % (c.max_containerPeriod - c.min_containerPeriod + 1);
                       
        int timeInterval = dropTime / containers[j].height; // Time interval for height decrease
       
        // Simulate the time taken to drop each container
       while (containers[j].height > 0) {
    sleep(timeInterval);
    containers[j].height -= 10;
    if (containers[j].height < 0) {
        containers[j].height = 0; // Correcting for overshoot below zero.
    }
    //printf("Container %d height decreased to %d\n", containers[j].containerID, containers[j].height);
}
      if (containers[j].destroyed == 0){
        printf("Container %d is dropped successfully\n", containers[j].containerID);
        containers[j].dropped = 1;
        containers[j].height = 0;
    }

  detach_shared_memory(containers);
}
}


void leave_and_refill() {
  // Simulate leaving the airspace and going for a re-fill
  int refillTime = c.min_containerRefill +
                   rand() % (c.max_containerRefill - c.min_containerRefill + 1);
  printf("Cargo Plane leaves northern Gaza airspace for re-fill...\n");
  printf("Re-fill period: %d seconds\n", refillTime);
  sleep(refillTime); // Simulate the time taken for re-filling
  printf("Cargo Plane returns after re-fill.\n");

  // Start cargo plane behavior again after re-fill
  cargo_plane_behavior(plane_id);
}
