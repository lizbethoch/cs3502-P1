//Lizbeth Ochoa
//CS3502 - Section W03
//P1

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_ACCOUNTS 2
#define INITIAL_BALANCE 1000
#define NUM_TRANSFERS 10

typedef struct {
    int id;
    double balance;
    pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];


// Lock ordering strategy
void safe_transfer_ordered(int from, int to, double amount) {
    //Step 1: Identify which account ID is lower
    int first = (from < to) ? from : to;
    int second = (from < to) ? to : from;
    //Step 2: Lock loer ID first
    pthread_mutex_lock(&accounts[first].lock);
    //Step 3: Loch higher ID second
    pthread_mutex_lock(&accounts[second].lock);

    //Step 4: Perform transfer
    if (accounts[from].balance >= amount) {

        accounts[from].balance -= amount;
        accounts[to].balance += amount;

        printf("Thread %ld transferred %.2f from %d -> %d\n",
               pthread_self(), amount, from, to);

    } else {

        printf("Thread %ld insufficient funds in account %d\n",
               pthread_self(), from);
    }
    //Step 5: Unlock in reverse order
    pthread_mutex_unlock(&accounts[second].lock);
    pthread_mutex_unlock(&accounts[first].lock);
}


typedef struct {
    int from;
    int to;
} TransferArgs;

void* transfer_worker(void* arg) {

    TransferArgs* data = (TransferArgs*)arg;

    for (int i = 0; i < NUM_TRANSFERS; i++) {

        safe_transfer_ordered(data->from, data->to, 10);

        usleep(1000);
    }

    return NULL;
}

void init_accounts() {

    for (int i = 0; i < NUM_ACCOUNTS; i++) {

        accounts[i].id = i;
        accounts[i].balance = INITIAL_BALANCE;

        pthread_mutex_init(&accounts[i].lock, NULL);
    }
}

void print_balances() {

    printf("\nFinal Account Balances:\n");

    for (int i = 0; i < NUM_ACCOUNTS; i++) {

        printf("Account %d: %.2f\n",
               accounts[i].id,
               accounts[i].balance);
    }
}


int main() {

    printf("Initializing accounts...\n");

    init_accounts();

    pthread_t t1, t2;

    TransferArgs a1 = {0,1};
    TransferArgs a2 = {1,0};

    printf("\nStarting transfers using Lock Ordering...\n");

    pthread_create(&t1, NULL, transfer_worker, &a1);
    pthread_create(&t2, NULL, transfer_worker, &a2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    print_balances();

    return 0;
}
