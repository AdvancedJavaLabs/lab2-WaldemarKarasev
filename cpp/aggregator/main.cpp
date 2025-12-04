#include "aggregator.hpp"

#include <thread>

int main()
{
    return aggregator::App{std::thread::hardware_concurrency() * 4}.Run();
}