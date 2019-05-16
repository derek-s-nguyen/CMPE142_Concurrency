#include <err.h>
#include <stdio.h>
#include <pthread.h>
#include <machine/atomic.h>

typedef struct
__lock_t
{
	int flag;
} lock_t;

void
init(lock_t *mutex)
{
	// 0 -> lock is available, 1 -> held
	mutex->flag = 0;
}

void
lock(lock_t *mutex)
{
	while (_atomic_cas_uint(&mutex->flag, 0, 1) == 1)
		; // spin
}

void
unlock(lock_t *mutex)
{
	mutex->flag = 0;
}

int counter = 0;
lock_t mutex;

void *
thread_function(void *argument)
{
	int i;

	for (i = 0; i < 10000000; i++) {
		lock(&mutex);
		counter++;
		unlock(&mutex);
	}

	return NULL;
}

int
main(int argc, char **argv)
{
	pthread_t thread1, thread2;

	init(&mutex);

	if (pthread_create(&thread1, NULL, thread_function, NULL))
		err(1, "pthread_create 1");

	if (pthread_create(&thread2, NULL, thread_function, NULL))
		err(1, "pthread_create 2");

	printf("waiting for threads\n");

	if(pthread_join(thread1, NULL))
		err(1, "pthread_join 1");

	if(pthread_join(thread2, NULL))
		err(1, "pthread_join 2");

	/* XXX - should we acquire the lock here? */
	printf("counter value = %d\n", counter);

	return 0;
}
