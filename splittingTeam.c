#include "containers.h"
#include "header.h"
#include "read.h"
#include "semaphores.h"

#define storage_key 0x1240


Config c;
int team_id;

void split_into_bags();

int main(int argc, char *argv[]) {

  c = read_config("userdefined.txt");

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <team_id>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  team_id = atoi(argv[1]);
  
  while(1){
  split_into_bags( );
  sleep(2);
  
  }

  return 0;
}
void split_into_bags() {

  // attach to storage shared memory to take content from it it

  int shmid = shmget(storage_key, sizeof(Storage), 0666);
  if (shmid < 0 ) {
    perror("shmget 31");
    exit(EXIT_FAILURE);
  }

  Storage *storage = (Storage *)shmat(shmid, NULL, 0);
  if (storage == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
    
    
  }
   
   while(storage->flourStorage == 0){
   }
   // Calculate the number of 1Kg bags that can be made from the flour in storage
    int bags_to_create = storage->flourStorage;
    
   // get semaphore, lock shared memory to update this container's value to be
  // TAKEN.
  sem_t *storage_mutex = get_semaphore(storage_sem_key);
  //printf("aaaaaa,team %d\n", team_id);
  lock_sem(storage_mutex);
  //printf("bbbbbb,team %d\n", team_id);

    // Update the shared memory with the number of new bags
    storage->bags_number += bags_to_create;

    // Decrease the flour storage as we have converted it into bags
    storage->flourStorage -= bags_to_create;
    
   unlock_sem(storage_mutex);
  
  // Close the semaphore here, after it's completely done being used
  close_semaphore(storage_mutex);

    
    printf("Splitter Team created %d bags from the flour storage.\n", storage->bags_number);
    detach_shared_memory(storage);

    
}
 

