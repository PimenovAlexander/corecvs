//
// Created by dio on 8/7/20.
//

#ifndef LIBEVENTSERVER_STATSGENERATOR_H
#define LIBEVENTSERVER_STATSGENERATOR_H

#include <stdlib.h>

int getRoll() { return rand() % 9000 - 4500; }
int getPitch() { return rand() % 9000 - 4500; }
int getYaw() { return rand() % 9000 - 4500; }

int param1 = 1;
int param2 = 2;
int param3 = 3;

int getParam1() { return param1; }
int getParam2() { return param2; }
int getParam3() { return param3; }

void setParam1(int val) { param1 = val; }
void setParam2(int val) { param2 = val; }
void setParam3(int val) { param3 = val; }

#endif //LIBEVENTSERVER_STATSGENERATOR_H
