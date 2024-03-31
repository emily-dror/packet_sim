#ifndef _SIM_HPP_
#define _SIM_HPP_

#include "event.hpp"

#include <vector>
#include <queue>

class simulation_c {
public:
    // Singleton
    static simulation_c &get_instance();
    simulation_c(const simulation_c& other) = delete;
    simulation_c &operator=(const simulation_c& other) = delete;
    ~simulation_c();

    // simulation methods, run by order.
    void parse_command_line(int argc, char* argv[]);
    void init();
	void run();

    // statistics (optional)
    void print_statistics();

    // used by events
    void finish_event(unsigned int exit_port);
    void schedule_event(unsigned int entry_port);

private:
    // Singleton
	simulation_c();

    // simulation's time
    double __time;

    // queue for each exit port
    std::deque<std::deque<event_c *>> __queues;

    // priority queue for events
	std::priority_queue<event_c *, std::vector<event_c *>, eventComparator> __event_queue;

    // total waiting time for processed events. used by print_statistics().
    double __waiting_time_sum;

    // total service time for processed events. used by print_statistics().
    double __service_time_sum;

    // packets dropped per exit port.
    std::vector<unsigned int> __packets_dropped;

    // packets delivered per exit port.
    std::vector<unsigned int> __packets_delivered;

    // private method. not for user.
    int choose_queue(int entry_port);

private:
    /* parse command line targets. */

    // time limit until no packets arrive (T)
    double __time_limit;

    // number of entry ports (N)
    unsigned int __entry_ports_count;

    // number of exit ports (M)
    unsigned int __exit_ports_count;

    // probability matrix (𝑃0,0 𝑃0,1 … 𝑃0,𝑀−1  𝑃1,0 𝑃1,1 … 𝑃1,𝑀−1  …  𝑃𝑁−1,0 𝑃𝑁−1,1 … 𝑃𝑁−1,𝑀−1)
    double** __probability;

     // arrival rates of packets (𝜆0 𝜆1 … 𝜆𝑁−1)
    double* __arrival_rates;

    // exit ports' queues' sizes (𝑄0 𝑄1 … 𝑄𝑀−1)
    unsigned int* __queues_capacities;

    // service rates of packets (𝜆0 𝜆1 … 𝜆𝑁−1)
    double* __service_rates;
};

#endif  // _SIM_HPP_