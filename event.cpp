#include "event.hpp"
#include "sim.hpp"

event_c::event_c(double time):
    time(time) {}

arrival_event_c::arrival_event_c(double time, unsigned int entry_port):
    event_c(time),
    entry_port(entry_port) {}

void arrival_event_c::process_event()
{
    simulation_c& sim = simulation_c::get_instance();
    sim.schedule_event(entry_port);
}

leave_event_c::leave_event_c(double time, unsigned int exit_port):
    event_c(time),
    exit_port(exit_port) {}

void leave_event_c::process_event()
{
    simulation_c& sim = simulation_c::get_instance();
    sim.finish_event(this->exit_port);
}
