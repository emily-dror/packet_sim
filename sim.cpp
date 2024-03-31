#include "sim.hpp"

#include <iostream>
#include <iomanip>
#include <random>

simulation_c::simulation_c():
    __time(0), __event_queue()
{
    __waiting_time_sum = 0;
    __service_time_sum = 0;
}

simulation_c &simulation_c::get_instance() {
    static simulation_c instance;
    return instance;
}

void simulation_c::parse_command_line(int argc, char* argv[])
{
    unsigned int base     = 0;
    __time_limit          = std::stof(argv[++base]);
    __entry_ports_count   = std::atoi(argv[++base]);
    __exit_ports_count    = std::atoi(argv[++base]);

    __probability         = new double*[__entry_ports_count];
    for (int i = 0; i < __entry_ports_count; ++i) {
        __probability[i] = new double[__exit_ports_count];

        __probability[i][0] = 100 * std::stof(argv[++base]);
        for (int j = 1; j < __exit_ports_count; ++j) {
            __probability[i][j] = 100 * std::stof(argv[++base]) + __probability[i][j - 1];
        }
    }

    __arrival_rates = new double[__entry_ports_count];
    for (int i = 0; i < __entry_ports_count; ++i) {
        __arrival_rates[i] = std::stof(argv[++base]);
    }

    __queues_capacities = new unsigned int[__exit_ports_count];
    for (int i = 0; i < __exit_ports_count; ++i) {
        __queues_capacities[i] = std::atoi(argv[++base]);
    }

    __service_rates = new double[__exit_ports_count];
    for (int i = 0; i < __exit_ports_count; ++i) {
        __service_rates[i] = std::stof(argv[++base]);
    }
}

simulation_c::~simulation_c()
{
    // Simulation ended. release used resources.
    for (int i = 0; i < __entry_ports_count; ++i) {
        delete[] __probability[i];
    }
    delete[] __probability;
    delete[] __arrival_rates;
    delete[] __queues_capacities;
    delete[] __service_rates;
}

void simulation_c::init()
{
    // Generate events' arrival times
    for (unsigned int i = 0; i < __entry_ports_count; ++i)
    {
        std::random_device                      rd;
        std::mt19937                            rng(rd());
        std::exponential_distribution<double>   exp(__arrival_rates[i]);

        double arrival_time_sum = 0;
        while (true)
        {
            double new_arrival_time = exp(rng);
            arrival_time_sum += new_arrival_time;
            if (arrival_time_sum > __time_limit) {
                break;
            }
            __event_queue.push(new arrival_event_c(arrival_time_sum, i));
        }
    }

    for (unsigned int i = 0; i < __exit_ports_count; ++i)
    {
        __queues.push_back(std::deque<event_c *>());
        __packets_dropped.push_back(0);
        __packets_delivered.push_back(0);
    }
}

void simulation_c::run()
{
    while (!__event_queue.empty())
    {
        event_c *next_event = __event_queue.top();
        __time = next_event->time;

        __event_queue.pop();
        next_event->process_event();
        delete next_event;
    }
}

int simulation_c::choose_queue(int entry_port)
{
    // seed the RNG
    std::random_device                  rd;
    std::mt19937                        rng(rd());
    std::uniform_int_distribution<int>  distribution(1, 100);

    int value = distribution(rng);
    for (int i = 0; i < __exit_ports_count; ++i) {
        if (value <= __probability[entry_port][i]) {
            return i;
        }
    }
    return -1;
}

void simulation_c::schedule_event(unsigned int entry_port)
{
    unsigned int exit_port = choose_queue(entry_port);
    if (__queues[exit_port].size() >= __queues_capacities[exit_port]) {
        ++__packets_dropped[exit_port];
        return;
    }

    std::random_device                    rd;
    std::mt19937                          rng(rd());
    std::exponential_distribution<double> exp(__service_rates[exit_port]);

    double time_processed;
    double service_time = exp(rng);
    if (__queues[exit_port].empty()) {
        time_processed = __time + service_time;
    } else {
        time_processed = __queues[exit_port].back()->time + service_time;
    }

    event_c *event = new leave_event_c(time_processed, exit_port);
    __queues[exit_port].push_back(event);
    __event_queue.push(event);

    // Event arrived at __time and processed at time_processed then
    // it waited for (time_processed - __time)
    __service_time_sum += service_time;
    __waiting_time_sum += time_processed - __time;
    ++__packets_delivered[exit_port];
}

void simulation_c::finish_event(unsigned int exit_port)
{
    __queues[exit_port].pop_front();
}

#define FIXED_FLOAT(x) std::fixed << std::setprecision(2) << (x)
void simulation_c::print_statistics()
{
    unsigned int total_packets_delivered = 0;
    for (int i = 0; i < __packets_delivered.size(); ++i) {
        total_packets_delivered += __packets_delivered[i];
    }

    std::cout << total_packets_delivered << " ";
    for (int i = 0; i < __packets_delivered.size(); ++i) {
        std::cout << __packets_delivered[i] << " ";
    }

    unsigned int total_packets_dropped = 0;
    for (int i = 0; i < __packets_dropped.size(); ++i) {
        total_packets_dropped += __packets_dropped[i];
    }

    std::cout << total_packets_dropped << " ";
    for (int i = 0; i < __packets_dropped.size(); ++i) {
        std::cout << __packets_dropped[i] << " ";
    }

    std::cout << FIXED_FLOAT(__time) << " ";
    std::cout << FIXED_FLOAT(__waiting_time_sum / total_packets_delivered) << " ";
    std::cout << FIXED_FLOAT(__service_time_sum / total_packets_delivered) << std::endl;
}
