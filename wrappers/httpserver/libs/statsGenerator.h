#ifndef LIBEVENTSERVER_STATSGENERATOR_H
#define LIBEVENTSERVER_STATSGENERATOR_H

//
// Created by dio on 8/7/20.
//


#include <stdlib.h>

int getRoll();
int getPitch();
int getYaw();

int getParam1();
int getParam2();
int getParam3();

void setParam1(int val);
void setParam2(int val);
void setParam3(int val);

#endif //LIBEVENTSERVER_STATSGENERATOR_H
