//
// Created by dio on 7/26/20.
//

#ifndef LIBEVENTAPP_SERVER_H
#define LIBEVENTAPP_SERVER_H

#include "libs/imageGenerator.h"
#include "libs/statsGenerator.h"
#include "libEventServer.h"

#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <chrono>

#include <iterator>
#include <vector>
#include <sstream>

#include "libs/base64.h"
#include "libEventServer.h"

extern LibEventServer *server;

void on_get_index(struct evhttp_request *req, void *arg);
void on_get_test_data(struct evhttp_request *req, void *arg);
void on_image_request(struct evhttp_request *req, void *arg);
void on_get_stats_request(struct evhttp_request *req, void *arg);
void on_change_stats_request(struct evhttp_request *req, void *arg);
void on_other_requests(struct evhttp_request * req, void *arg);

int startWServer();

#endif //LIBEVENTAPP_SERVER_H
