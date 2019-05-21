#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t customer = PTHREAD_MUTEX_INITIALIZER;

//chef and customers
pthread_cond_t chef_c = PTHREAD_COND_INITIALIZER;
pthread_cond_t customer_hamburger_c = PTHREAD_COND_INITIALIZER;
pthread_cond_t customer_fries_c = PTHREAD_COND_INITIALIZER;
pthread_cond_t customer_soda_c = PTHREAD_COND_INITIALIZER;

//conditionals
pthread_cond_t hamburger = PTHREAD_COND_INITIALIZER;
pthread_cond_t fries = PTHREAD_COND_INITIALIZER;
pthread_cond_t soda = PTHREAD_COND_INITIALIZER;

bool have_hamburger = false;
bool have_fries = false;
bool have_soda = false;

int chef_job = 1;
int customer_hamburger_job = 0;
int customer_fries_job = 0;
int customer_soda_job = 0;

int counter = 0;
int hamburgerCounter = 0, friesCounter = 0, sodaCounter = 0;

void *chef(void *arg) {

	while (1) {

		sleep(1);

		pthread_mutex_lock(&m);

		//The chef stays waiting if chef_job is equal 0
		while (chef_job == 0) pthread_cond_wait(&chef_c, &m);

		printf("------------------------------------------------------------\n \n");

		counter++;

		if (counter < 101) printf("Counter: %d \n", counter);
		
		//output at the end of the code
		if (counter == 101) {
			printf("TOTAL:\n");
			printf("Hamburger customer count: %d\n", hamburgerCounter);
			printf("Fries customer count: %d\n", friesCounter);
			printf("Soda customer count: %d\n", sodaCounter);
			exit(0);

		}
		//randomized number generator
		int randNum = rand() % 3;

		//fries and hamburgers
		if (randNum == 0) {

			chef_job = 0;
			have_hamburger = true;
			have_fries = true;
			sodaCounter++;
			puts("Chef puts fries and hamburger");
			pthread_cond_signal(&fries);
			pthread_cond_signal(&hamburger);

		}

		//if soda and burgers
		else if (randNum == 1) {
			chef_job = 0;
			have_hamburger = true;
			have_soda = true;
			friesCounter++;
			puts("Chef puts soda and hamburger");
			pthread_cond_signal(&soda);
			pthread_cond_signal(&hamburger);
		}

		//if soda and fries
		else if (randNum == 2) {
			chef_job = 0;
			have_soda = true;
			have_fries = true;
			hamburgerCounter++;
			puts("Chef puts fries and soda");
			pthread_cond_signal(&fries);
			pthread_cond_signal(&soda);
		}

		pthread_mutex_unlock(&m);
	}
	return 0;
}

//all pusher functions are here

void *pusher_fries(void *arg){
	while (1) {
		pthread_mutex_lock(&m);
		while (!have_fries) pthread_cond_wait(&fries, &m);
		if (have_hamburger) {
			have_hamburger = false;
			chef_job = 0;
			customer_soda_job = 1;
			puts("Call the soda customer");
			pthread_cond_signal(&customer_soda_c);
		}
		if (have_soda) {
			have_soda = false;
			chef_job = 0;
			customer_hamburger_job = 1;
			puts("Call the hamburger customer");
			pthread_cond_signal(&customer_hamburger_c);
		}
		pthread_mutex_unlock(&m);
	}
	return 0;
}
void *pusher_hamburger(void *arg) {

	while (1) {
		pthread_mutex_lock(&m);
		while (!have_hamburger)
			pthread_cond_wait(&hamburger, &m);

		if (have_fries) {
			have_fries = false;
			chef_job = 0;
			customer_soda_job = 1;
			puts("Call the soda customer");
			pthread_cond_signal(&customer_soda_c);
		}
		if (have_soda) {
			have_soda = false;
			chef_job = 0;
			customer_fries_job = 1;
			puts("Call the fries customer");
			pthread_cond_signal(&customer_fries_c);
		}
		pthread_mutex_unlock(&m);
	}

	return 0;
}

void *pusher_soda(void * arg){
    while(1){
        pthread_mutex_lock(&m);
        while(!have_soda) pthread_cond_wait(&soda, &m);

        if(have_hamburger){
            have_hamburger = false;
            chef_job = 0;
            customer_fries_job = 1;
            puts("Call the fries customer");
            pthread_cond_signal(&customer_fries_c);
        }
        if(have_fries){
            have_fries = false;
            chef_job = 0;
            customer_hamburger_job = 1;
            puts("Call the hamburger customer");
            pthread_cond_signal(&customer_hamburger_c);
        }
        pthread_mutex_unlock(&m);
    }
    return 0 ;
}

//all the functions that deal with the threee customers

void * customer_soda(void * arg){
    
    while(1){
        
        pthread_mutex_lock(&customer);
        while(customer_soda_job == 0)
            pthread_cond_wait(&customer_soda_c, &customer);
        have_fries = false;
        have_hamburger = false;
        customer_soda_job = 0;
        chef_job = 1;
        puts("Soda customer takes fries and hamburger then eats");
        pthread_mutex_unlock(&customer);
        
        puts("Soda customer rings bell");
    }
    
    return 0;
}
void *customer_hamburger(void *arg) {

	while (1) {

		pthread_mutex_lock(&customer);
		while (customer_hamburger_job == 0)
			pthread_cond_wait(&customer_hamburger_c, &customer);
		have_fries = false;
		have_soda = false;
		customer_hamburger_job = 0;
		chef_job = 1;
		puts("Hamburger customer takes fries and soda then eats");
		pthread_mutex_unlock(&customer);

		puts("Hamburger customer rings bell");
	}

	return 0;
}
void * customer_fries(void * arg) {

	while (1) {
		pthread_mutex_lock(&customer);
		while (customer_fries_job == 0) pthread_cond_wait(&customer_fries_c, &customer);
		have_soda = false;
		have_hamburger = false;
		customer_fries_job = 0;
		chef_job = 1;
		puts("Fries customer takes hamburger and soda then eats");
		pthread_mutex_unlock(&customer);

		puts("Fries customer rings bell");
	}
	return 0;
}

int main(int argc, char *argv[]) {
	pthread_t chef_t, customer_soda_t, customer_fries_t, customer_hamburger_t, pusher_soda_t, pusher_fries_t, pusher_hamburger_t;

	//random seed
	time_t t;

	srand((unsigned) time(&t));

	if (pthread_create(&chef_t, NULL, chef, NULL) != 0) {
		fprintf(stderr, "Impossible to create thread\n");
		exit(1);
	}

	if (pthread_create(&pusher_soda_t, NULL, pusher_soda, NULL) != 0) {
		fprintf(stderr, "Thread cannot be created\n");
		exit(1);
	}

	if (pthread_create(&pusher_fries_t, NULL, pusher_fries, NULL) != 0) {
		fprintf(stderr, "Thread cannot be created\n");
		exit(1);
	}

	if (pthread_create(&pusher_hamburger_t, NULL, pusher_hamburger, NULL)
			!= 0) {
		fprintf(stderr, "Thread cannot be created\n");
		exit(1);
	}

	if (pthread_create(&customer_soda_t, NULL, customer_soda, NULL) != 0) {
		fprintf(stderr, "Thread cannot be created\n");
		exit(1);
	}

	if (pthread_create(&customer_fries_t, NULL, customer_fries, NULL) != 0) {
		fprintf(stderr, "Thread cannot be created\n");
		exit(1);
	}

	if (pthread_create(&customer_hamburger_t, NULL, customer_hamburger, NULL)
			!= 0) {
		fprintf(stderr, "Thread cannot be created\n");
		exit(1);
	}

	pthread_join(chef_t, NULL);
	pthread_join(pusher_soda_t, NULL);
	pthread_join(pusher_fries_t, NULL);
	pthread_join(pusher_hamburger_t, NULL);
	pthread_join(customer_soda_t, NULL);
	pthread_join(customer_fries_t, NULL);
	pthread_join(customer_hamburger_t, NULL);
}
