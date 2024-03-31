#ifndef _SIM_HPP_
#define _SIM_HPP_

#include "event.hpp"

#include <vector>
#include <queue>
#include <array>
#include <memory>


class simulation_c {
public:
    static simulation_c &get_instance();
	void run();
    int  choose_queue(int entry_port);
    void set_probability(double **probability);
    void set_service_rates(double *service_rates);
    void set_queues(unsigned int *queues_capacities);
    void finish_event(event_c *event, unsigned int exit_port);
    void schedule_event(unsigned int exit_port);
    void generate_arrival_times(double time_limit, double *arrival_rates);
    void set_ports(unsigned int entry_ports_count, unsigned int exit_ports_count);

    void print_statistics();

private:
	simulation_c();
    void generate_arrival_times(double time_limit, double arrival_rate, unsigned int entry_port);

    double time;
    double **probability;
    double *service_rates;
    unsigned int exit_ports_count;
    unsigned int entry_ports_count;
    unsigned int *queues_capacities;
    std::deque<std::deque<event_c *>> queues;
	std::priority_queue<event_c *, std::vector<event_c *>, eventComparator> eventQueue;

    double waiting_time_sum;
    double service_time_sum;
    std::vector<unsigned int> packets_dropped;
    std::vector<unsigned int> packets_delivered;
};

#endif  // _SIM_HPP_