#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

void *add_func(void* args);
void *dec_func(void* args);
pthread_mutex_t mutex;
pthread_cond_t cond;
sem_t sem;
int count = 0;
int add_times = 0;
int dec_times = 0;

int main()
{
	pthread_t pid1, pid2;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	sem_init(&sem, 0, 0);
	int ret;

	timeval tv1, tv2;
	gettimeofday(&tv1, NULL);

	ret = pthread_create(&pid2, NULL, dec_func, NULL);
	if (ret != 0)
	{
		printf("can not create write_funx!\r\n");
	}

	ret = pthread_create(&pid1, NULL, add_func, NULL);
	if (ret != 0)
	{
		printf("can not create read_funx!\r\n");
	}

	pthread_join(pid1, NULL);
	pthread_join(pid2, NULL);

	gettimeofday(&tv2, NULL);
	int total = (tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec);
	printf("total time = %d\n", total);

	printf("count now %d\r\n", count);
	return 0;
}
void *add_func(void* args)
{
	for (;;)
	{
#if 1
		pthread_mutex_lock(&mutex);
		count++;
		add_times++;
		pthread_cond_signal(&cond);	
		pthread_mutex_unlock(&mutex);
#endif

#if 0
		pthread_mutex_lock(&mutex);
		add_times++;
		count++;
		pthread_mutex_unlock(&mutex);
		sem_post(&sem);
#endif
		if (add_times == 50000)
		{
			printf("add exit now add_times is %d\n", add_times);
			return NULL;
		}
	}
}
void *dec_func(void* args)
{
	for (;;)
	{
#if 1
		pthread_mutex_lock(&mutex);
		while (count == 0)
		{
			pthread_cond_wait(&cond, &mutex);
		}
		--count;
		++dec_times;
		pthread_mutex_unlock(&mutex);
#endif

#if 0
		sem_wait(&sem);
		pthread_mutex_lock(&mutex);
		dec_times++;
		count--;
		pthread_mutex_unlock(&mutex);
#endif

		if (dec_times == 50000)
		{
			printf("write exit now add_times is %d\n", dec_times);
			return NULL;
		}
	}
}