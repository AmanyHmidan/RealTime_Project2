#include "containers.h"
#include "header.h"
#include "read.h"
#include "semaphores.h"
#include "local.h"

typedef struct {
    int starvation_rate;
    int family_pid;
} FamilyData;

Config c;
int team_id;
//int numFamilies = 5;

// Custom comparison function for qsort to sort FamilyData by starvation_rate in descending order
int compare(const void *a, const void *b) {
    FamilyData *dataA = (FamilyData *)a;
    FamilyData *dataB = (FamilyData *)b;
    return dataB->starvation_rate - dataA->starvation_rate;
}

void distribute_into_families(int team_id);

int main(int argc, char *argv[]) {
    c = read_config("userdefined.txt");
    printf("from distribution: num of families = %d\n", c.numFamilies);

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <team_id>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    team_id = atoi(argv[1]);
    

    while (1) {
        distribute_into_families(team_id);
    }

    return 0;
}

void distribute_into_families(int team_id) {
    int mid, n, num_rates = 0;
    MESSAGE msg;
    FamilyData family_data[c.numFamilies]; // Use FamilyData array

    if ((mid = msgget(Q1_KEY, 0)) == -1) {
        mid = msgget(Q1_KEY, IPC_CREAT | 0660);
    }

    sleep(5);

    while (num_rates < c.numFamilies) {
        if ((n = msgrcv(mid, &msg, sizeof(msg), DISTRIBUTION_TEAM_TYPE, 0)) == -1) {
            perror("Server: msgrcv");
            return;
        }

        int rate = *((int *)msg.buffer);
        printf("received rate = %d\n", rate);

        if (num_rates < c.numFamilies) {
            family_data[num_rates].starvation_rate = rate;
            family_data[num_rates].family_pid = msg.msg_fm;
            num_rates++;
        } else {
            fprintf(stderr, "Maximum number of families reached. Ignoring additional rates.\n");
        }

        qsort(family_data, num_rates, sizeof(FamilyData), compare);

        printf("Distribution Team: Starvation rates received from families (max to min):\n");
        for (int i = 0; i < num_rates; i++) {
            printf("Family PID: %d - Starvation Rate: %d\n", family_data[i].family_pid, family_data[i].starvation_rate);
        }
    }

    printf("All families have sent messages. Exiting...\n");
    int q2_id = msgget(Q2_KEY, IPC_CREAT | 0660);

    // Send family PIDs to workers
for (int i = 0; i < num_rates; i++) {
    MESSAGE msg;
    msg.msg_to = TO_WORKER_TYPE;             // Set recipient to worker
    msg.msg_fm = DISTRIBUTION_TEAM_TYPE;     // Set sender to distribution team
    
    snprintf(msg.buffer, BUFSIZ, "%d", family_data[i].family_pid);
     int n = sizeof(msg.msg_to) + sizeof(msg.msg_fm) + strlen(msg.buffer) + 1; // Calculate message size
   if (msgsnd(q2_id, &msg, n, 0) == -1) {   // Send message
        perror("msgsnd");
        return 6;
    }
    printf("Sent family PID %d to worker\n", family_data[i].family_pid);
    sleep(1); 
}
}
