#include <iostream>
#include <random>
#include <memory>

#include "TrafficLight.h"


/* Implementation of class "MessageQueue" */
 
template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> u_lock{ _mutex };
    _condition_var.wait(u_lock, [this] {return !_queue.empty();});
    T msg = std::move(_queue.back());
  	_queue.pop_back();

  	return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lock{_mutex};
    _queue.clear();
    _queue.emplace_back(msg);
    _condition_var.notify_one();
}

void TrafficLight::waitForGreen()
{
    while (true)
    {
        TrafficLightPhase msg = _msg_queue.receive();
        if (msg == TrafficLightPhase::green)
            break;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    std::lock_guard<std::mutex> lock{_mutex};
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread{&TrafficLight::cycleThroughPhases, this});
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    auto start_time = std::chrono::high_resolution_clock::now();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distribution(4.0, 6.0);
    double duration = distribution(gen);

    while (true)
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto delta_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
        if(delta_time >= duration)
        {
            duration = distribution(gen);
            start_time = std::chrono::high_resolution_clock::now();
            std::unique_lock<std::mutex> u_lock{_mutex};
            _currentPhase = _currentPhase == TrafficLightPhase::red ? TrafficLightPhase::green : TrafficLightPhase::red;
            u_lock.unlock();
            _msg_queue.send(std::move(_currentPhase));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
