#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

enum class TrafficLightPhase{red, green};

template <class T>
class MessageQueue
{
public:
    void send (T&& message)
    {
        std::lock_guard<std::mutex> myLock(_mutex);
        _msgs.push_front(std::move(message));
        _condition.notify_one();
    }

    T receive()
    {
        // Wait until msgs queue is not empty
        std::unique_lock<std::mutex> myLock(_mutex);
        _condition.wait(myLock, [this] (){return !_msgs.empty();});
        T msg = std::move(_msgs.back());
        _msgs.pop_back();
        return msg;
    }

private:
    std::condition_variable _condition;
    std::mutex _mutex;
    std::deque<T> _msgs;
};

class TrafficLight : TrafficObject
{
public:
    
    TrafficLight() : _currentPhase(TrafficLightPhase::red){}

    void simulate();
    void waitForGreen();
    TrafficLightPhase getCurrentPhase() const
    {
        return _currentPhase;
    }

private:
    void cycleThroughPhases();

    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _lightPhases;
};

#endif