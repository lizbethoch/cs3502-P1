//Lizbeth Ochoa
//CS 3502 - Section W03
//P1

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <unistd.h>

// Configuration - expirement with different values!
#define NUM_ACCOUNTS 2
#define NUM_THREADS 4
#define TRANSACTIONS_PER_THREAD 10
#define INITIAL_BALANCE 1000.0

// Account data structure (GIVEN)
typedef struct {
	int account_id;
	double balance;
	int transaction_count;
} Account;

// Global shared array - THIS CAUSES RACECONDITIONS!
Account accounts [NUM_ACCOUNTS];

// GIVEN : Example deposit function WITH race condition
void deposit_unsafe(int account_id, double amount) {
	// READ
	double current_balance = accounts[account_id].balance;

	// MODIFY (simulate processing time)
	usleep (1) ; // This increases likelihood of race condition!
	double new_balance = current_balance + amount;

	// WRITE (another thread might have changed balance between READ an /WRITE !)
	accounts[account_id].balance = new_balance;
	accounts[account_id].transaction_count++;
}

// TODO 1: Implement withdrawal_unsafe() following the same pattern
// Reference: Copy the structure of deposit_unsafe() above
// Question: What 's different between deposit and withdrawal?
void withdrawal_unsafe (int account_id , double amount) {
	// YOUR CODE HERE
	// Hint : READ current balance
	double current_balance = accounts[account_id].balance;

	usleep(1);
	// Hint : SUBTRACT amount instead of add
	double new_balance = current_balance - amount;

	// Hint : WRITE new balance
	accounts[account_id].balance = new_balance;
	accounts[account_id].transaction_count++;
}

// TODO 2: Implement the thread function
// Reference : See OSTEP Ch.27 for pthread function signature
// Reference : Appendix A.2 for void* parameter explanation
void* teller_thread(void* arg) {
	int teller_id = *(int*)arg; // GIVEN : Extract thread ID

	// TODO 2a: Initialize thread-safe random seed
	// Reference: Section 7.2 "Random Numbers per Thread"
	// Hint: unsigned int seed = time(NULL) ^ pthread_self();
	unsigned int seed = time(NULL) ^ pthread_self();

	for (int i = 0; i < TRANSACTIONS_PER_THREAD; i++) {
		// TODO 2b: Randomly select an account (0 to NUM_ACCOUNTS -1)
		// Hint: Use rand_r(&seed) % NUM_ACCOUNTS
		int account_idx = rand_r(&seed) % NUM_ACCOUNTS;

		// TODO 2c: Generate random amount (1 -100)
		double amount = (rand_r(&seed) % 100) + 1;

		// TODO 2d: Randomly choose deposit (1) or withdrawal (0)
		// Hint : rand_r(&seed) % 2
		int operation = rand_r(&seed) % 2;

		// TODO 2e: Call appropriate function
		if (operation == 1) {
			deposit_unsafe(account_idx, amount);
			printf("Teller %d: Deposited $%.2f to Account %d\n",
				teller_id, amount, account_idx);
		} else {
			// YOUR CODE HERE - call withdrawal_unsafe
			withdrawal_unsafe(account_idx, amount);
			printf("Teller %d: Withdrew $%.2f from Account %d\n",
				teller_id, amount, account_idx);
		}
	}

	return NULL ;
}

// TODO 3: Implement main function
// Reference : See pthread_create and pthread_join man pages
int main () {
	printf("=== Phase 1: Race Conditions Demo ===\n\n");

	// TODO 3a: Initialize all accounts
	// Hint: Loop through accounts array
	// Set: account_id = i, balance = INITIAL_BALANCE, transaction_count = 0

	for(int i = 0; i < NUM_ACCOUNTS; i++) {
		accounts[i].account_id = i;
		accounts[i].balance = INITIAL_BALANCE;
		accounts[i].transaction_count = 0;
	}

	// Display initial state (GIVEN)
	printf("Initial State:\n");
	for (int i = 0; i < NUM_ACCOUNTS; i++) {
		printf("Account %d: $%.2f\n", i, accounts[i].balance);
	}

	// TODO 3b: Calculate expected final balance
	// Question: With random deposits/withdrawals , what should total be?
	// Hint: Total money in system should remain constant!
	double expected_total = NUM_ACCOUNTS * INITIAL_BALANCE;

	printf("\nExpected total: $%.2f\n\n", expected_total);

	// TODO 3c: Create thread and thread ID arrays
	// Reference: man pthread_create for pthread_t type
	pthread_t threads[NUM_THREADS];
	int thread_ids[NUM_THREADS]; // GIVEN: Separate array for IDs

	// TODO 3d: Create all threads
	// Reference: man pthread_create
	// Caution: See Appendix A .2 warning about passing & i in loop!
	for (int i = 0; i < NUM_THREADS; i++) {
		thread_ids [i] = i; // GIVEN : Store ID persistently
		// YOUR pthread_create CODE HERE
		pthread_create(&threads[i], NULL, teller_thread, &thread_ids[i]);
	}

	// TODO 3e: Wait for all threads to complete
	// Reference: man pthread_join
	// Question : What happens if you skip this step ?
	for (int i = 0; i < NUM_THREADS; i++) {
		// YOUR pthread_join CODE HERE
		pthread_join(threads[i], NULL);
	}

	// TODO 3f: Calculate and display results
	printf("\n=== Final Results ===\n");
	double actual_total = 0.0;

	for (int i = 0; i < NUM_ACCOUNTS; i++) {
		printf("Account %d: $%.2f (%d transactions)\n",
			i, accounts[i].balance, accounts[i].transaction_count);
		actual_total += accounts[i].balance;
	}

	printf("\nExpected total: $%.2f\n", expected_total);
	printf("Actual total: $%.2f\n", actual_total);
	printf("Difference: $%.2f\n", actual_total - expected_total);

	// TODO 3g: Add race condition detection message
	// If expected != actual, print "RACE CONDITION DETECTED!"
	// Instruct user to run multiple times

	if(actual_total != expected_total) {
		printf("\nRACE CONDITION DETECTED!\n");
		printf("Run multiple times to see different results.\n");
	}

	return 0;
}


