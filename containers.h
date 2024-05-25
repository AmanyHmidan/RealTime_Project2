#ifndef CONTAINERS_H
#define CONTAINERS_H

#include "header.h"

#define Plane_key 0x1236
#define totalContainers_key 0x1238
#define Container_key 0x1239
#define storage_key 0x1240
#define family_key 0x1241
#define Q1_KEY 0x1247
#define Q2_KEY 0x1243
#define Workers_KEY 0x1245
#define Number_of_Workers_KEY 0x1246

typedef struct {
   int containerID;
    int height;
    int taken; 
    int content;
    int dropped; 
    int hit;
    int destroyed;  
} Container;

typedef struct {
    int planeID;
    int numContainers;
   Container *containers;  
} Plane;

typedef struct{
    int totalContainers;
    int lastContainer_id;

}totalContainers;

typedef struct{

   int flourStorage; // flour weight in Kg
   int bags_number;
   int bags_worker[];

}Storage;


typedef struct { 
    int id;
    int starvationRate;
    int numOfBags;
    int died; 
} Family; // Define a struct to hold information about families 


typedef struct{

 int pid;
 int energy;
 int killed;
 int team; // 0 for collecting team, 1 for distribution team


}Workers;


typedef struct{

 int num_collectingWorkers;
 int num_splittingWorkers;
 int num_distributingWorkers;
 
 int num_Killed_collectingWorkers;
 int num_Killed_distributingWorkers;
 
 int total_C_killed;
 int total_D_killed;


}Number_of_Workers;
 


// Function Declarations
//Connecter *attach_shared_memory(int shmid);
Plane *attach_shared_memory(int shmid);
void detach_shared_memory(void *shmaddr);
void destroy_shared_memory(int shmid);
int create_shared_memory(int MAX_PLANES);


#endif

