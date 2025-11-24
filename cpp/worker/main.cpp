#include "worker.hpp"

// std
#include <thread>

int main()
{
    return worker::App{std::thread::hardware_concurrency() * 2}.Run();
}
