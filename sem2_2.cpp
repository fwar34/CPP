#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <list>

using namespace std;

static list<int> fifo;
static pthread_mutex_t mutex;
static sem_t sem;

void* tfun1(void* arg);
void* tfun2(void* arg);

int main(int argc, char **argv)
{
    pthread_t t1, t2;

    pthread_mutex_init(&mutex, NULL);
    sem_init(&sem, 0, 0);

    pthread_create(&t1, NULL, tfun1, NULL);
    pthread_create(&t2, NULL, tfun2, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem);

    return 0;
}

void* tfun1(void* arg)
{
    int num = 0;
    while (true)
    {
        pthread_mutex_lock(&mutex);
        fifo.push_back(num);
        sem_post(&sem);
        printf("push element(%d) to fifo\n", num++);
        pthread_mutex_unlock(&mutex);
    }
}

void* tfun2(void* arg)
{
    int num = 0;
    while (true)
    {
        sem_wait(&sem);
        pthread_mutex_lock(&mutex);
        while (!fifo.empty())
        {
            num = fifo.front();
            fifo.pop_front();
            printf("get element(%d) from fifo\n", num);
        }
        /*num = fifo.front();
        fifo.pop_front();
        printf("get element(%d) from fifo\n", num);*/
        pthread_mutex_unlock(&mutex);
    }
}
