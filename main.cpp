#include "sim.hpp"

int main(int argc, char* argv[])
{
    simulation_c& sim = simulation_c::get_instance();

    // Parse command line
    sim.parse_command_line(argc, argv);

    // Initialize sim's parameters
    sim.init();

    // Run the simulation.
    sim.run();

    // Print simulation statistics.
    sim.print_statistics();
    return 0;
}
