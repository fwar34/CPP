#include <pthread.h>
#include <stdio.h>
#include <list>

using namespace std;

list<int> fifo;

pthread_cond_t cond;
pthread_mutex_t mutex;


void* thread_fun1(void* arg)
{
    int num1 = 0;
    while (true)
    {
        pthread_mutex_lock(&mutex);
        fifo.push_back(num1);
        printf("push element(%d) to fifo\n", num1++);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    /*while (true)
    {
        pthread_mutex_lock(&mutex);
        if (fifo.empty())
        {
            fifo.push_back(num1);
            printf("push element(%d) to fifo\n", num1++);
            pthread_cond_signal(&cond);
        }
        pthread_mutex_unlock(&mutex);
    }*/
}

void* thread_fun2(void* arg)
{
    int num2 = 0;

    while (true)
    {
        pthread_mutex_lock(&mutex);
        while (fifo.empty())
        {
            pthread_cond_wait(&cond, &mutex);
        }
        while (!fifo.empty())
        {
            num2 = fifo.front();
            fifo.pop_front();
            printf("get element(%d) from fifo\n", num2);
        }
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char **argv)
{
    pthread_t t1, t2;

    printf("fifo init size = %d\n", fifo.size());

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&t1, NULL, thread_fun1, NULL);
    pthread_create(&t2, NULL, thread_fun2, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
