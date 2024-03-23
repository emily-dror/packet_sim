#include "event.hpp"
#include "sim.hpp"

#if !defined(NDEBUG)
#include <iostream>
#endif  // NDEBUG

void arrival_event_c::process_event()
{
    simulation_c& sim = simulation_c::get_instance();
    unsigned int exit_port = sim.choose_queue(entry_port);
    sim.schedule_event(exit_port, this->time);

#if !defined(NDEBUG)
    std::cout << "arrival_event_c is processed: " << time << '\n';
#endif  // NDEBUG
}


void leave_event_c::process_event()
{
    simulation_c& sim = simulation_c::get_instance();
    sim.finish_event(this, this->exit_port);

#if !defined(NDEBUG)
    std::cout << "leave_event_c is done: " << time << '\n';
#endif  // NDEBUG
}
