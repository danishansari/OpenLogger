#include <iostream>

#include <unistd.h>

#include "OpenLogger.h"

using namespace std;

void* threadFun1(void *);
void* threadFun2(void *);

OpenLogger logger;

int main()
{
    logger.setProperty("LOG_TIME");
    logger.setProperty("ENABLE_THREAD");
    logger.log(INF, "main started\n");

    pthread_t t1, t2;
    pthread_create(&t1, NULL, threadFun1, NULL);
    pthread_create(&t2, NULL, threadFun2, NULL);
  
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}

void* threadFun2(void *)
{
    return NULL;
    OpenLogger logger("thread_2.log");
    cout << "Thread func2 ::: ::: ::: " << endl;
    for (int i = 0; i < 1000; i++)
    {
        if (i%2 == 0)
            logger.e("ThreadFunc2:: i = %d \n", i);
        else if(i%3 == 0)
            logger.w("ThreadFunc2:: i = %d \n", i);
        else if(i%5 == 0)
            logger.i("ThreadFunc2:: i = %d \n", i);
        else
            logger.d("ThreadFunc2:: i = %d \n", i);

        usleep(10000);
    }
}

void* threadFun1(void *)
{
    int count = 0;
    cout << "Thread func1 ::: ::: ::: " << endl;
    for (int i = 0; i < 1000;)
    {
        if (i%2 == 0)
            logger.e("ThreadFunc1:: i = %d \n", i);
        else if(i%3 == 0)
            logger.w("ThreadFunc1:: i = %d \n", i);
        else if(i%5 == 0)
            logger.i("ThreadFunc1:: i = %d \n", i);
        else
            logger.d("ThreadFunc1:: i = %d \n", i);
        
        if (count++ > 5)
        {
            i++;
            count = 0;
        }
        usleep(10000);
    }

    return NULL;
}
