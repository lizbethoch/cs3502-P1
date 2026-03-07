//Lizbeth Ochoa
//CS 3502 - Section W03
//P1

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_ACCOUNTS 2
#define NUM_TELLERS 4
#define NUM_TRANSACTIONS 10
#define INITIAL_BALANCE 1000.0

// Account structure with mutex
typedef struct {
    int account_id;
    double balance;
    int transaction_count;
    pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];


// GIVEN: Example of mutex initialization
void initialize_accounts() {
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].account_id = i;
        accounts[i].balance = INITIAL_BALANCE;
        accounts[i].transaction_count = 0;

        pthread_mutex_init(&accounts[i].lock, NULL);
    }
}


void deposit_safe(int account_id, double amount) {

    pthread_mutex_lock(&accounts[account_id].lock);

    // ===== CRITICAL SECTION =====
    accounts[account_id].balance += amount;
    accounts[account_id].transaction_count++;
    // ============================

    pthread_mutex_unlock(&accounts[account_id].lock);
}


// TODO 1: Implemen withdrawal_safe() with mutex protection
void withdrawal_safe(int account_id, double amount) {

    pthread_mutex_lock(&accounts[account_id].lock);

    // ===== CRITICAL SECTION =====
        accounts[account_id].balance -= amount;
        accounts[account_id].transaction_count++;
    // ============================

    pthread_mutex_unlock(&accounts[account_id].lock);
}


// Teller thread function
void* teller_thread(void* arg) {

    for (int i = 0; i < NUM_TRANSACTIONS; i++) {

        int account_id = rand() % NUM_ACCOUNTS;
        double amount = rand() % 100;

        if (rand() % 2 == 0)
            deposit_safe(account_id, amount);
        else
            withdrawal_safe(account_id, amount);
    }

    return NULL;
}


// Cleanup mutexes
void cleanup_mutexes() {
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }
}


int main() {

    pthread_t tellers[NUM_TELLERS];

    struct timespec start, end;

    initialize_accounts();

    // TODO 3: Performance timing start
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create threads
    for (int i = 0; i < NUM_TELLERS; i++) {
        pthread_create(&tellers[i], NULL, teller_thread, NULL);
    }

    // Wait for threads
    for (int i = 0; i < NUM_TELLERS; i++) {
        pthread_join(tellers[i], NULL);
    }

    // End timing
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("\nFinal Account Balances:\n");

    double total = 0;

    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("Account %d: %.2f (Transactions: %d)\n",
               accounts[i].account_id,
               accounts[i].balance,
               accounts[i].transaction_count);

        total += accounts[i].balance;
    }

    printf("\nTotal Balance: %.2f\n", total);
    printf("Execution Time: %f seconds\n", elapsed);

    // TODO 4: Cleanup mutexes
    cleanup_mutexes();

    return 0;
}
