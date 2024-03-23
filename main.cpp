#include "sim.hpp"

#include <iostream>
#include <ctime>
#include <deque>

void parse_cmd_line(char* argv[], float &time_limit, unsigned int &entry_ports_count,
                                unsigned int &exit_ports_count, float** &probability,
                                float* &arrival_rates, unsigned int* &queues_capacities,
                                float* &service_rates)
{
    unsigned int base   = 0;
    time_limit          = std::stof(argv[++base]);
    entry_ports_count   = std::atoi(argv[++base]);
    exit_ports_count    = std::atoi(argv[++base]);

    probability         = new float*[entry_ports_count];
    for (int i = 0; i < entry_ports_count; ++i) {
        probability[i] = new float[exit_ports_count];

        probability[i][0] = 100 * std::stof(argv[++base]);
        for (int j = 1; j < exit_ports_count; ++j) {
            probability[i][j] = 100 * std::stof(argv[++base]) + probability[i][j - 1];
        }
    }

    arrival_rates = new float[entry_ports_count];
    for (int i = 0; i < entry_ports_count; ++i) {
        arrival_rates[i] = std::stof(argv[++base]);
    }

    queues_capacities = new unsigned int[exit_ports_count];
    for (int i = 0; i < exit_ports_count; ++i) {
        queues_capacities[i] = std::atoi(argv[++base]);
    }

    service_rates = new float[exit_ports_count];
    for (int i = 0; i < exit_ports_count; ++i) {
        service_rates[i] = std::stof(argv[++base]);
    }
}

int main(int argc, char* argv[]) {

    // Get starting timepoint
    time_t start, finish;
    time(&start);

    // time limit until no packets arrive (T)
    float time_limit;

    // number of entry ports (N)
    unsigned int entry_ports_count;

    // number of exit ports (M)
    unsigned int exit_ports_count;

    // probability matrix (𝑃0,0 𝑃0,1 … 𝑃0,𝑀−1  𝑃1,0 𝑃1,1 … 𝑃1,𝑀−1  …  𝑃𝑁−1,0 𝑃𝑁−1,1 … 𝑃𝑁−1,𝑀−1)
    float** probability;

     // arrival rates of packets (𝜆0 𝜆1 … 𝜆𝑁−1)
    float *arrival_rates;

    // exit ports' queues' sizes (𝑄0 𝑄1 … 𝑄𝑀−1)
    unsigned int *queues_capacities;

    // service rates of packets (𝜆0 𝜆1 … 𝜆𝑁−1)
    float *service_rates;

    parse_cmd_line(argv, time_limit, entry_ports_count, exit_ports_count, probability,
                                            arrival_rates, queues_capacities, service_rates);

#if !defined(NDEBUG)
    std::cout << "T = " << time_limit << std::endl;
    std::cout << "N = " << entry_ports_count << std::endl;
    std::cout << "M = " << exit_ports_count << std::endl;

    std::cout << "Probabilities = ";
    for (int i = 0; i < entry_ports_count; ++i) {
        for (int j = 0; j < exit_ports_count; ++j) {
            std::cout << probability[i][j] << " ";
        }
    }
    std::cout << std::endl;

    std::cout << "Arrival rates = ";
    for (int i = 0; i < entry_ports_count; ++i) {
        std::cout << arrival_rates[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Exit ports' queues' sizes = ";
    for (int i = 0; i < exit_ports_count; ++i) {
        std::cout << queues_capacities[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Service rates = ";
    for (int i = 0; i < exit_ports_count; ++i) {
        std::cout << service_rates[i] << " ";
    }
    std::cout << std::endl;
#endif  // NDEBUG

#if !defined(NDEBUG)
    std::cout << "\n- Packet Switch simulation, Computer Networks hw4\n";
#endif  // NDEBUG

    simulation_c& sim = simulation_c::get_instance();

    // Set entry & exit ports' counts.
    sim.set_ports(entry_ports_count, exit_ports_count);

    // Set probability.
    sim.set_probability(probability);

    // Set queues' service rates.
    sim.set_service_rates(service_rates);

    // Set queues' capacities.
    sim.set_queues(queues_capacities);

    // Generate exponentially-distributed arrival times.
    sim.generate_arrival_times(time_limit, arrival_rates);

    // Run the simulation.
    sim.run();

    // Get ending timepoint
    time(&finish);

    // Print simulation statistics.
    sim.print_statistics(difftime(finish, start));

#if !defined(NDEBUG)
    std::cout << "Simulation ended.\n";
#endif  // NDEBUG

    // Simulation ended. release used resources.
    for (int i = 0; i < entry_ports_count; ++i) {
        delete[] probability[i];
    }
    delete[] probability;
    delete[] arrival_rates;
    delete[] queues_capacities;
    delete[] service_rates;
    return 0;
}
