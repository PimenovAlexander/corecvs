//
// Created by dio on 8/21/20.
//

#include "longPoll.h"

#include <utility>

using namespace corecvs;

/**
 * Add a subscriber that should receive a reply at the next instance of certain event happening
 * @param event name of the event
 * @param client request from the subscriber
 */
void LongPoll::subscribe(const std::string& event, subscriber client)
{
    SYNC_PRINT(("LongPoll::subscribe(const std::string& event, subscriber client) : called\n"));
    events[event].subscribers.push(client);
}

/**
 * Adds an event so that users can subscribe to it
 * @param event name of the event
 * @param callback function that has to be invoked for each subscriber of this event when it happens
 * @param provider object that has to supply content
 * @param url url for which to construct content
 * @return 0 on success, -1 if event's name matches an existing event
 */
int LongPoll::addEvent(const std::string& event, callback_fn callback, ContentProvider *provider, const std::string& url)
{
    SYNC_PRINT(("LongPoll::addEvent(const std::string& event, callback_fn callback) : called\n"));

    if (events.find(event) != events.end())
        return -1; // List of events already contains an event with the same title

    auto& ev = events[event];

    ev = long_poll_event();
    ev.subscribers = std::queue<subscriber>();
    ev.callback = callback;
    ev.provider = provider;
    ev.url = url;

    return 0;
}

/**
 * Notifies every subscriber of an event by calling an appropriate callback function.
 * This function also unsubscribes everyone from the event in question
 * @param event name of the event
 */
void LongPoll::announce(const std::string& event)
{
    SYNC_PRINT(("LongPoll::announce(const std::string& event) : called\n"));

    auto ev = events[event];

    auto subscribers = &ev.subscribers;
    auto callback = ev.callback;
    auto provider = ev.provider;
    auto url = ev.url;

    while (!subscribers->empty())
    {
        (*callback)(subscribers->front(), provider->getContent(url));   // Run callback function for each subscriber
        subscribers->pop();                                             // Unsubscribe a client
    }

    events[event].ready = false;
}

/**
 * Marks the event but doesn't immediately send replies to subscribers.
 * @param event name of the event
 */
void LongPoll::notice(const std::string& event)
{
    SYNC_PRINT(("LongPoll::notice(const std::string& event) : called\n"));
    events[event].ready = true;
}

/**
 * Processing all events by calling callbacks for each event marked as "Ready" and for each subscriber
 * of that event.
 */
void LongPoll::process()
{
    SYNC_PRINT(("LongPoll::process() : called\n"));
    auto event = events.begin();
    while(event != events.end())
    {
        if (event->second.ready)
        {
            announce(event->first);
        }
        event++;
    }
}
