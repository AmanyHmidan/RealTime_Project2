#include "containers.h"
#include "header.h"
#include "read.h"
#include "semaphores.h"

Config c;
int team_id;

void collecting_team_behavior(int team_id);
void add_toStorge(int value, int team_id);
void retrunTo_function(int team_id);

int main(int argc, char *argv[]) {

  c = read_config("userdefined.txt");

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <team_id>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  team_id = atoi(argv[1]);

  collecting_team_behavior(team_id);

  return 0;
}

void collecting_team_behavior(int team_id) {

  // access totalContainers shared memory to get total number of containers
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

  //detach_shared_memory(shared_data);

  // access the shared memory of containers to collect one of them

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

  // the team will only handle one container
  // look for the first contianer found with taken value = 0 and dropped = 1 ,
  // which means it is dropped and not handled yet
  int ourContainer_id;
  int found_container = 0;

  while (1) {
    for (int i = 1; i <= shared_data->totalContainers; i++) {

      if (containers[i].taken == 0 && containers[i].dropped == 1) {
        ourContainer_id = containers[i].containerID;
        found_container = 1;
        printf("Container %d to collect is found from Team %d\n",
               ourContainer_id, team_id);
        break;
      }
    }

    if (found_container) {
      break; // Exit the while loop if a container is found
    }
  }

  // get semaphore, lock shared memory to update this container's value to be
  // TAKEN.
  sem_t *container_mutex = get_semaphore(containers_sem_key);
  //printf("aaaaaa,team %d\n", team_id);
  lock_sem(container_mutex);
  //printf("bbbbbb,team %d\n", team_id);

  if (containers[ourContainer_id].taken == 0) {
    // Mark the container as taken
    containers[ourContainer_id].taken = 1;

    // Unlock the semaphore after updating the container
    unlock_sem(container_mutex);

    // Close the semaphore after completely using it
    // close_semaphore(container_mutex); // Don't close here

    printf("Team %d took container %d\n", team_id, ourContainer_id);

    int value = containers[ourContainer_id].content;

    // Move the detachment after accessing container content
    detach_shared_memory(containers);

    // Add the container content to storage
    add_toStorge(value, team_id);
  } else {
    // If the container was already taken, release the semaphore and continue
    unlock_sem(container_mutex);
  }

  // Close the semaphore here, after it's completely done being used
  close_semaphore(container_mutex);

  retrunTo_function( team_id);
}

void retrunTo_function(int team_id) { collecting_team_behavior(team_id); }

void add_toStorge(int value, int team_id) {

  // attach to storage shared memory to add content to it

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

  storage->flourStorage += value;

  printf("value of storage = %d,team = %d\n", storage->flourStorage, team_id);

  detach_shared_memory(storage);

  // recall the function to collect another container
  collecting_team_behavior(team_id);
}
