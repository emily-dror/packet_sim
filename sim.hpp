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
    void set_probability(float **probability);
    void set_service_rates(float *service_rates);
    void set_queues(unsigned int *queues_capacities);
    void finish_event(event_c *event, unsigned int exit_port);
    void schedule_event(unsigned int exit_port, float time_anchor);
    void generate_arrival_times(float time_limit, float *arrival_rates);
    void set_ports(unsigned int entry_ports_count, unsigned int exit_ports_count);

    void print_statistics(const double& elapsed_seconds);

private:
	simulation_c();
    float generate_finish_time(int exit_port);
    void  generate_arrival_times(float time_limit, float arrival_rate, unsigned int entry_port);

    float time;
    float **probability;
    float *service_rates;
    unsigned int exit_ports_count;
    unsigned int entry_ports_count;
    unsigned int *queues_capacities;
    std::vector<std::deque<event_c *>> queues;
	std::priority_queue<event_c *, std::vector<event_c *>, eventComparator> eventQueue;

    std::vector<unsigned int> packets_dropped;
    std::vector<unsigned int> packets_delivered;
};

#endif  // _SIM_HPP_