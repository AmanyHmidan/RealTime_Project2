#ifndef LOCAL_H
#define LOCAL_H

// Define message types
#define FAMILY_TYPE 1
#define DISTRIBUTION_TEAM_TYPE 2
#define TO_WORKER_TYPE 3

// Define a seed for ftok()
#define SEED 123

// Define message structure
typedef struct {
    long msg_to;    // Message recipient
    long msg_fm;    // Message sender
    char buffer[BUFSIZ]; // Message data buffer
} MESSAGE;

#endif /* LOCAL_H */

