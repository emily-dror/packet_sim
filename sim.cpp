#include "sim.hpp"

#include <iostream>
#include <iterator>
#include <random>

simulation_c::simulation_c():
    time(0), eventQueue()
{
#if !defined(NDEBUG)
    std::cout << "sim c'tor\n";
#endif  // NDEBUG
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

#if !defined(NDEBUG)
    std::cout << "queues size = " << queues.size() << '\n';
#endif  // NDEBUG
}

void simulation_c::set_probability(float **probability) {
    this->probability = probability;
}

void simulation_c::set_service_rates(float *service_rates) {
    this->service_rates = service_rates;
}

void simulation_c::set_queues(unsigned int *queues_capacities) {
    this->queues_capacities = queues_capacities;
}

void simulation_c::generate_arrival_times(float time_limit, float *arrival_rates) {
    for (unsigned int i = 0; i < entry_ports_count; ++i) {
        generate_arrival_times(time_limit, arrival_rates[i], i);
    }
}

void simulation_c::generate_arrival_times(float time_limit, float arrival_rate, unsigned int entry_port)
{
#if !defined(NDEBUG)
        std::cout << "generate_arrival_times entry_port = " << entry_port << '\n';
#endif  // NDEBUG

    std::random_device                      rd;                     // uniformly-distributed integer random number generator
    std::mt19937                            rng(rd());              // mt19937: Pseudo-random number generation
    std::exponential_distribution<float>    exp(1 / arrival_rate);  // exponentially-distributed random number generator

    float arrival_time_sum = 0;
    while (true)
    {
        float new_arrival_time = exp(rng);                          // generates the diff to next arrival
        arrival_time_sum += new_arrival_time;
        if (arrival_time_sum > time_limit) {
            break;
        }
        eventQueue.push(new arrival_event_c(arrival_time_sum, entry_port));

#if !defined(NDEBUG)
        std::cout << "new arrival_event added " << arrival_time_sum << '\n';
#endif  // NDEBUG
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

float simulation_c::generate_finish_time(int exit_port)
{
    std::random_device                      rd;                                 // uniformly-distributed integer random number generator
    std::mt19937                            rng(rd());                          // mt19937: Pseudo-random number generation
    std::exponential_distribution<float>    exp(1 / service_rates[exit_port]);  // exponentially-distributed random number generator

    if (queues[exit_port].empty()) {
        return exp(rng);                                                        // generates the diff to next arrival
    }
    return queues[exit_port].back()->time + exp(rng);                           // generates the diff to next arrival
}

void simulation_c::schedule_event(unsigned int exit_port, float time_anchor)
{
    if (queues[exit_port].size() > queues_capacities[exit_port]) {
        ++packets_dropped[exit_port];
        return;
    }
    ++packets_delivered[exit_port];
    event_c *event = new leave_event_c(time_anchor + generate_finish_time(exit_port), exit_port);
    queues[exit_port].push_back(event);
    eventQueue.push(event);
}

void simulation_c::finish_event(event_c *event, unsigned int exit_port)
{
    if (event->time != queues[exit_port].front()->time) {
        return;
    }
    queues[exit_port].pop_front();
}

void simulation_c::run()
{
    while(!eventQueue.empty()) {
        event_c *next_event = eventQueue.top();
        this->time = next_event->time;

#if !defined(NDEBUG)
        if (dynamic_cast<arrival_event_c *>(next_event)) {
            std::cout << "\nnext_event is arrival_event_c: " << next_event->time << '\n';
        } else {
            std::cout << "\nnext_event is leave_event_c: " << next_event->time << '\n';
        }
#endif  // NDEBUG

        eventQueue.pop();
        next_event->process_event();
        delete next_event;
    }
}

void simulation_c::print_statistics(const double& elapsed_seconds)
{
    unsigned int sum = 0;
    for (int i = 0; i < packets_delivered.size(); ++i) {
        sum += packets_delivered[i];
    }

    std::cout << sum << " ";
    for (int i = 0; i < packets_delivered.size(); ++i) {
        std::cout << packets_delivered[i] << " ";
    }

    for (int i = 0; i < packets_dropped.size(); ++i) {
        sum += packets_dropped[i];
    }

    std::cout << sum << " ";
    for (int i = 0; i < packets_dropped.size(); ++i) {
        std::cout << packets_dropped[i] << " ";
    }

    std::cout << "\ntime " << elapsed_seconds << " ";
}
