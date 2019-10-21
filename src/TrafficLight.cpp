#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <chrono>

// Method will return when traffic light is green
// If the light is not green calling thread will be blocked 
void TrafficLight::waitForGreen()
{
    while (true)
    {
        TrafficLightPhase currPhase = _lightPhases.receive();
        if (currPhase == TrafficLightPhase::green)
        {
            return;
        }       
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase() const 
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    
    // Create a random duration of light between 4-6 seconds
    unsigned  seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(4,6);
    int duration = distribution(generator);
    
    auto startOfCycle = std::chrono::steady_clock::now();
    while (true)
    {
        auto currTime = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(currTime - startOfCycle).count() >= duration)
        {
            // Reset the the start of cycle time for the next iteration
            startOfCycle = std::chrono::steady_clock::now();
            // It is now time to cycle the light
            _currentPhase = (_currentPhase == TrafficLightPhase::green)? TrafficLightPhase::red : TrafficLightPhase::green;     
            _lightPhases.send(std::move(TrafficLightPhase(_currentPhase)));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

}

