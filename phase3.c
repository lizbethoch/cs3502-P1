//Lizbeth ochoa
//CS 3502 - Section W03
//P1

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_ACCOUNTS 2
#define INITIAL_BALANCE 1000

typedef struct {
    int account_id;
    double balance;
    pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];

volatile int progress_made = 0;

/* Initialize accounts */
void initialize_accounts() {
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].account_id = i;
        accounts[i].balance = INITIAL_BALANCE;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }
}

//GIVEN: Conceptual example showing HOW deadlock occurs
void transfer_deadlock_example(int from_id, int to_id, double amount) {

    pthread_mutex_lock(&accounts[from_id].lock);
    printf("Thread %ld: Locked account %d\n", pthread_self(), from_id);

    //Simulate processing delay
    usleep(100000);
    //Try to lock destination account
    printf("Thread %ld: Waiting for account %d\n", pthread_self(), to_id);

    pthread_mutex_lock(&accounts[to_id].lock);   // DEADLOCK HERE!

    //Transfer (never reached if deadlocked)
        accounts[from_id].balance -= amount;
        accounts[to_id].balance += amount;

    progress_made = 1;

    pthread_mutex_unlock(&accounts[to_id].lock);
    pthread_mutex_unlock(&accounts[from_id].lock);
}


//Thread 1: transfer (0, 1, amount)
void* thread_one(void* arg) {

    while (1) {
        transfer_deadlock_example(0, 1, 100);
        sleep(1);
    }

    return NULL;
}

//Thread 2 : transfer (1, 0, amount)
void* thread_two(void* arg) {

    while (1) {
        transfer_deadlock_example(1, 0, 100);
        sleep(1);
    }

    return NULL;
}


int main() {

    pthread_t t1, t2;

    initialize_accounts();

    printf("\nStarting deadlock demonstration...\n\n");

    pthread_create(&t1, NULL, thread_one, NULL);
    pthread_create(&t2, NULL, thread_two, NULL);

    //Deadlock
    time_t start = time(NULL);

    while (1) {

        sleep(1);

        if (progress_made) {
            start = time(NULL);
            progress_made = 0;
        }

        if (time(NULL) - start > 5) {
            printf("\n============================\n");
            printf("SUSPECTED DEADLOCK DETECTED\n");
            printf("No progress detected for 5 seconds\n");
            printf("============================\n");
            break;
        }
    }

    printf("\nProgram terminating after deadlock detection.\n");

    return 0;
}
