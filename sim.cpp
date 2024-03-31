#include "sim.hpp"

#include <iostream>
#include <iterator>
#include <iomanip>
#include <random>

simulation_c::simulation_c():
    time(0), eventQueue()
{
    waiting_time_sum = 0;
    service_time_sum = 0;
}

simulation_c &simulation_c::get_instance() {
    static simulation_c instance;
    return instance;
}

void simulation_c::set_ports(unsigned int entry_ports_count, unsigned int exit_ports_count)
{
    this->exit_ports_count = exit_ports_count;
    this->entry_ports_count = entry_ports_count;

    for (int i = 0; i < exit_ports_count; ++i) {
        queues.push_back(std::deque<event_c *>());
        packets_dropped.push_back(0);
        packets_delivered.push_back(0);
    }
}

void simulation_c::set_probability(double **probability) {
    this->probability = probability;
}

void simulation_c::set_service_rates(double *service_rates) {
    this->service_rates = service_rates;
}

void simulation_c::set_queues(unsigned int *queues_capacities) {
    this->queues_capacities = queues_capacities;
}

void simulation_c::generate_arrival_times(double time_limit, double *arrival_rates) {
    for (unsigned int i = 0; i < entry_ports_count; ++i) {
        generate_arrival_times(time_limit, arrival_rates[i], i);
    }
}

void simulation_c::generate_arrival_times(double time_limit, double arrival_rate, unsigned int entry_port)
{
    std::random_device                      rd;                     // uniformly-distributed integer random number generator
    std::mt19937                            rng(rd());              // mt19937: Pseudo-random number generation
    std::exponential_distribution<double>    exp(arrival_rate);  // exponentially-distributed random number generator

    double arrival_time_sum = 0;
    while (true)
    {
        double new_arrival_time = exp(rng);                          // generates the diff to next arrival
        arrival_time_sum += new_arrival_time;
        if (arrival_time_sum > time_limit) {
            break;
        }
        eventQueue.push(new arrival_event_c(arrival_time_sum, entry_port));
    }
}

int simulation_c::choose_queue(int entry_port)
{
    // seed the RNG
    std::random_device                  rd;                     // uniformly-distributed integer random number generator
    std::mt19937                        rng(rd());              // mt19937: Pseudo-random number generation
    std::uniform_int_distribution<int>  distribution(1, 100);   // uniformly-distributed random number generator

    int value = distribution(rng);
    for (int i = 0; i < exit_ports_count; ++i) {
        if (value <= probability[entry_port][i]) {
            return i;
        }
    }
    return -1;
}

void simulation_c::schedule_event(unsigned int exit_port)
{
    if (queues[exit_port].size() >= queues_capacities[exit_port]) {
        ++packets_dropped[exit_port];
        return;
    }

    std::random_device                          rd;                                 // uniformly-distributed integer random number generator
    std::mt19937                                rng(rd());                          // mt19937: Pseudo-random number generation
    std::exponential_distribution<double>       exp(service_rates[exit_port]);      // exponentially-distributed random number generator

    double time_processed;
    double service_time = exp(rng);
    if (queues[exit_port].empty()) {
        time_processed = this->time + service_time;
    } else {
        time_processed = queues[exit_port].back()->time + service_time;
    }

    event_c *event = new leave_event_c(time_processed, exit_port);
    queues[exit_port].push_back(event);
    eventQueue.push(event);

    // Event arrived at this->time and processed at time_processed then
    // it waited for (time_processed - this->time)
    service_time_sum += service_time;
    waiting_time_sum += time_processed - this->time;
    ++packets_delivered[exit_port];
}

void simulation_c::finish_event(event_c *event, unsigned int exit_port) {
    queues[exit_port].pop_front();
}

void simulation_c::run()
{
    while(!eventQueue.empty()) {
        event_c *next_event = eventQueue.top();
        this->time = next_event->time;
        eventQueue.pop();
        next_event->process_event();
        delete next_event;
    }
}

#define FIXED_FLOAT(x) std::fixed << std::setprecision(2) << (x)

void simulation_c::print_statistics()
{
    unsigned int total_packets_delivered = 0;
    for (int i = 0; i < packets_delivered.size(); ++i) {
        total_packets_delivered += packets_delivered[i];
    }

    std::cout << total_packets_delivered << " ";
    for (int i = 0; i < packets_delivered.size(); ++i) {
        std::cout << packets_delivered[i] << " ";
    }

    unsigned int total_packets_dropped = 0;
    for (int i = 0; i < packets_dropped.size(); ++i) {
        total_packets_dropped += packets_dropped[i];
    }

    std::cout << total_packets_dropped << " ";
    for (int i = 0; i < packets_dropped.size(); ++i) {
        std::cout << packets_dropped[i] << " ";
    }

    std::cout << FIXED_FLOAT(this->time) << " ";
    std::cout << FIXED_FLOAT(waiting_time_sum / total_packets_delivered) << " ";
    std::cout << FIXED_FLOAT(service_time_sum / total_packets_delivered) << " ";
}
