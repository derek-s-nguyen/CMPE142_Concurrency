#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<semaphore.h>
#include<pthread.h>
int table_used = 1, cooked_items[2], generated = 0;
int customer_count[3];
char *food_items[]={"cooked a Hamburger","made some Fries","poured a Soda"};
sem_t table;
void *chef_boi(void *arg){
    int food_item_1, food_item_2;
    int make_100_meals = 0;
    while(1)
    {
	sleep(1);
	srand((unsigned)time(0));
        sem_wait(&table);
        if(make_100_meals == 100){
	    printf("\nTOTALS:\n");
	    printf("_______________________ \n");
            printf("Customer 1 ate %i meals \n", customer_count[0]);
            printf("Customer 2 ate %i meals \n", customer_count[1]);
            printf("Customer 3 ate %i meals \n", customer_count[2]);
            exit(0); 
        } 
        if(table_used == 1){
            do{
	        food_item_1 = rand() % 3;
		food_item_2 = rand() % 3;
	    }while(food_item_1 == food_item_2);
            cooked_items[0] = food_item_1;
            cooked_items[1] = food_item_2;
            printf("Chef has %s and %s", food_items[food_item_1], food_items[food_item_2]);
            generated = 1;
            table_used = 0;
            make_100_meals = make_100_meals + 1;
        }
        sem_post(&table);
    }
}
void *customer_boi(void *i){
    while(1){
	sleep(1);
        sem_wait(&table);
        if(table_used == 0){
            if(generated && cooked_items[0] != i && cooked_items[1] != i){
                printf(", so customer %d has eaten \n",(int)i+1);
                printf("\n");
                customer_count[(int)i]++;
                table_used = 1;
                generated = 0;
            }
        }
    sem_post(&table);
    }
}

int main(){
    pthread_t customer_1, customer_2, customer_3, chef;
    sem_init(&table, 0, 1);
    printf("Customer 1 has a Hamburger\n");
    printf("Customer 2 has Fries\n");
    printf("Customer 3 has a Soda\n");
    pthread_create(&chef, 0, chef_boi, 0);
    pthread_create(&customer_1, 0, customer_boi, (void*)0);
    pthread_create(&customer_2, 0, customer_boi, (void*)1);
    pthread_create(&customer_3, 0, customer_boi, (void*)2);
    while(1);
}

