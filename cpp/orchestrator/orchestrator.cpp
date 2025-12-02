#include "orchestrator.hpp"

// std
#include <csignal>
#include <fstream>
#include <iostream>
#include <thread>
#include <sys/wait.h>

int Orchestrator::Run()
{
    StartProcess("./aggregator", {}, "aggregator");

    for (size_t i = 0; i < workers_; ++i)
    {
        StartProcess("./worker", {}, "worker_" + std::to_string(i));
    }

    std::cout << "Orchestrator started. Press Ctrl+C to stop.\n";


    std::string consumer_tag = client_.Subscribe(tp::RabbitClient::GetMetricQueueName(), 10);
    while (s_is_running_)
    {
        auto messages = client_.ConsumeBatch(consumer_tag, 10);
        MessageProcessing(messages);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        ReapChildrenNonblocking();
    }


    ReapChildrenNonblocking();

    std::cout << "Orchestrator: stopping, sending SIGTERM to all children..." << std::endl;
    SendSignalToAll(SIGTERM);

    WaitAllChildren();
    std::cout << "Orchestrator: all children stopped, exiting." << std::endl;
    return 0;
}



// fork + exec
pid_t Orchestrator::StartProcess(const std::string& path, const std::vector<std::string>& args, const std::string& name)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return -1;
    }

    if (pid == 0)
    {
        // CHILD

        // ignore SIGINT
        std::signal(SIGINT, SIG_IGN);

        // prep argv for execvp
        std::vector<char*> argv;
        argv.reserve(args.size() + 2);
        argv.push_back(const_cast<char*>(path.c_str()));
        for (const auto& a : args)
            argv.push_back(const_cast<char*>(a.c_str()));
        argv.push_back(nullptr);

        execvp(path.c_str(), argv.data());
        // exec error
        perror("execvp");
        _exit(1);
    }

    // PARENT
    std::cout << "Started " << name << " pid=" << pid << " (" << path << ")" << std::endl;
    children_.push_back({pid, name});
    return pid;
}

void Orchestrator::SendSignalToAll(int sig)
{
    for (const auto& child : children_)
    {
        if (child.pid > 0)
        {
            std::cout << "Sending signal " << sig << " to " 
                      << child.name << " (pid=" << child.pid << ")" << std::endl;
            kill(child.pid, sig);
        }
    }
}

void Orchestrator::ReapChildrenNonblocking()
{
    while (true)
    {
        int status = 0;
        pid_t pid = waitpid(-1, &status, WNOHANG);
        if (pid <= 0)
            break;

        for (auto it = children_.begin(); it != children_.end(); ++it)
        {
            if (it->pid == pid)
            {
                std::cout << "Child " << it->name << " (pid=" << pid 
                          << ") exited with status " << status << "\n";
                children_.erase(it);
                break;
            }
        }
    }
}

// Waiting for shutdows
void Orchestrator::WaitAllChildren()
{
    while (!children_.empty())
    {
        int status = 0;
        pid_t pid = waitpid(-1, &status, 0);
        if (pid <= 0)
            break;

        for (auto it = children_.begin(); it != children_.end(); ++it)
        {
            if (it->pid == pid)
            {
                std::cout << "Child " << it->name << " (pid=" << pid 
                          << ") exited with status " << status << "\n";
                children_.erase(it);
                break;
            }
        }
    }
}


void Orchestrator::MessageProcessing(std::vector<tp::RabbitClient::Message> messages)
{
    for (const auto& msg : messages)
    {
        tp::Metric metric = tp::Metric::from_json(msg.body);

        if (awaiting_metrics_.contains(metric.id))
        {
            auto it = awaiting_metrics_[metric.id];

            metric.elapsed_time -= it.elapsed_time;
            
            WriteMetricToFile(metric);
            
            awaiting_metrics_.erase(metric.id);
        }
        else
        {
            if (metric.tag != tp::Metric::Tag::END)
            {
                awaiting_metrics_.insert({metric.id, metric});
            }
            else
            {
                metric.elapsed_time = tp::Metric::ms{0};
                WriteMetricToFile(metric);
            }
        }

        client_.Ack(msg.delivery);
    }    
}

static std::filesystem::path dir = "results";
void Orchestrator::WriteMetricToFile(const tp::Metric& metric)
{
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directory(dir);
    }

    std::filesystem::path filename = dir 
                                        / std::filesystem::path(std::string("metric_id") + std::to_string(metric.id));
    std::ofstream file(filename);

    if (file.is_open())
    {
        file << tp::Metric::to_json(metric) << std::flush;
    }

}