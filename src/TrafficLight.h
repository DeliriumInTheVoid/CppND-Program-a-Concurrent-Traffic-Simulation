#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

enum class TrafficLightPhase : uint
{
    red = 0,
    green,
};

template <class T>
class MessageQueue
{
public:
    void send(T&& msg);
    T receive();
private:
    std::deque<T> _queue{};
    std::mutex _mutex{};
    std::condition_variable _condition_var{};
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();
private:
    // typical behaviour methods
    void cycleThroughPhases();

    std::condition_variable _condition;
    std::mutex _mutex;

    TrafficLightPhase _currentPhase{ TrafficLightPhase::red };
    MessageQueue<TrafficLightPhase> _msg_queue{};
};

#endif