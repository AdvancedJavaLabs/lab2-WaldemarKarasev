
// std
#include <csignal>
#include <iostream>

// orchestrator
#include "orchestrator.hpp"

void sigint_handler(int)
{
    Orchestrator::StopApp();
}

int main(int argc, char const *argv[])
{
    std::signal(SIGINT, sigint_handler);

    if (argc < 2)
    {
        std::cout << "Usage: orchestrator <worker-count>" << std::endl;
        return 1; 
    }

    std::cout << "argc=" << argc << std::endl;

    try
    {
        size_t workers = std::stoul(argv[1]);
        return Orchestrator{workers}.Run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    
    return 0;
}