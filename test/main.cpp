#include <iostream>

#include "OpenLogger.h"

using namespace std;

int main()
{
    OpenLogger logger("myLog.log");
    logger.log(WAR, "hello\n");

    int x = 5;

    logger.e("hello %d \n", x);
    logger.i("hello\n");
    logger.d("hello\n");
    logger.v("hello\n");

    return 0;
}
