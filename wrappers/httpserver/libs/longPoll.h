//
// Created by dio on 8/21/20.
//

#ifndef CORECVS_LONGPOLL_H
#define CORECVS_LONGPOLL_H

#include <event2/event.h>
#include <event2/http.h>

#include "core/utils/global.h"

#include <queue>
#include <map>

#include "core/utils/utils.h"
#include "httpUtils.h"
#include "modules/contentProvider.h"
#include "memory"

using callback_fn = void (*)(evhttp_request *, const std::shared_ptr<HttpContent>&);
typedef evhttp_request* subscriber;

struct long_poll_event {
    std::queue<subscriber> subscribers;
    std::string url;
    callback_fn callback = nullptr;
    ContentProvider *provider = nullptr;
    bool ready = false;
};

class LongPoll {
public:
    LongPoll() = default;
    void subscribe(const std::string& event, subscriber client);
    int addEvent(const std::string& event, callback_fn callback, ContentProvider *provider, const std::string& url);
    void announce(const std::string& event);
    void notice(const std::string& event);
    void process();
private:
    std::map<std::string, long_poll_event> events;
};


#endif //CORECVS_LONGPOLL_H
