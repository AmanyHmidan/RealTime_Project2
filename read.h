#include "header.h"

typedef struct {
    
    int num_splittingWorkers;
    int num_distributingWorkers;
    int threshold_starveToDie;
    int num_collectingTeams;
    int num_cargoPlanes;
    int min_containersNum;
    int max_containersNum;
    int containersWeight;
    int min_containerPeriod;
    int max_containerPeriod;
    int numFamilies;
    int max_starvation;
    int min_starvation;
    int bags_eachWorker;
    int min_containerRefill;
    int max_containerRefill;  
    int ALTITUDE_THRESHOLD; 
    int initial_ALTITUDE;
    int num_collectingWorkers;
    int max_energy;
    int min_energy;
    int endGame_deadCollectors;
    int endGame_deadDistributers;
    int bags_eachFamily;
    int threshold_deadDistributers;
    int endGame_deadFamilies;
    int endGame_hitContainers;
    double Terminate_time;
    
} Config;

// Function to read config from file
Config read_config(const char *filename) {
    Config config = {0};
    FILE *file = fopen(filename, "r");

    if (file != NULL) {
        
        fscanf(file, "num_splittingWorkers = %d\n", &config.num_splittingWorkers);
        fscanf(file, "num_distributingWorkers = %d\n", &config.num_distributingWorkers);
        fscanf(file, "threshold_starveToDie = %d\n", &config.threshold_starveToDie);
        fscanf(file, "num_collectingTeams = %d\n", &config.num_collectingTeams);
        fscanf(file, "num_cargoPlanes = %d\n", &config.num_cargoPlanes);
        fscanf(file, "min_containersNum = %d\n", &config.min_containersNum);
        fscanf(file, "max_containersNum = %d\n", &config.max_containersNum);
        fscanf(file, "containersWeight = %d\n", &config.containersWeight);
        fscanf(file, "min_containerPeriod = %d\n", &config.min_containerPeriod);
        fscanf(file, "max_containerPeriod = %d\n", &config.max_containerPeriod);
        fscanf(file, "numFamilies = %d\n", &config.numFamilies);
        fscanf(file, "max_starvation = %d\n", &config.max_starvation);
        fscanf(file, "min_starvation = %d\n", &config.min_starvation);
        fscanf(file, "bags_eachWorker = %d\n", &config.bags_eachWorker);
        fscanf(file, "min_containerRefill = %d\n", &config.min_containerRefill);
        fscanf(file, "max_containerRefill = %d\n", &config.max_containerRefill);
        fscanf(file, "ALTITUDE_THRESHOLD = %d\n", &config.ALTITUDE_THRESHOLD);
        fscanf(file, "initial_ALTITUDE = %d\n", &config.initial_ALTITUDE);
        fscanf(file, "num_collectingWorkers = %d\n", &config.num_collectingWorkers);
        fscanf(file, "max_energy = %d\n", &config.max_energy);
        fscanf(file, "min_energy = %d\n", &config.min_energy);
        fscanf(file, "bags_eachFamily = %d\n", &config.bags_eachFamily);
        fscanf(file, "endGame_deadCollectors = %d\n", &config.endGame_deadCollectors);
        fscanf(file, "endGame_deadDistributers = %d\n", &config.endGame_deadDistributers);
        fscanf(file, "endGame_deadFamilies = %d\n", &config.endGame_deadFamilies);
        fscanf(file, "threshold_deadDistributers = %d\n", &config.threshold_deadDistributers);
        fscanf(file, "endGame_hitContainers = %d\n", &config.endGame_hitContainers);
        fscanf(file, "Terminate_time = %lf\n", &config.Terminate_time);
        
        
        
        
        fclose(file);
    } else {
        fprintf(stderr, "Error: Unable to open the configuration file.\n");
        exit(EXIT_FAILURE);
    }

    return config;
}


