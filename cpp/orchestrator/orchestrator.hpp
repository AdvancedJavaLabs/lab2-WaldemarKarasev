#pragma once

// std
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

// common
#include <common/rabbitmq_client.hpp>
#include <common/message.hpp>


class Orchestrator
{
private:
    struct ChildProcess
    {
        pid_t pid = -1;
        std::string name;
    };

public:
    Orchestrator(size_t workers) : workers_{workers} {}

    int Run();

    static void StopApp() { s_is_running_ = false; }

private:
    // process handling
    pid_t StartProcess(const std::string& path, const std::vector<std::string>& args, const std::string& name);
    void SendSignalToAll(int sig);
    void ReapChildrenNonblocking();
    void WaitAllChildren();

private:
    // Metric handling
    void MessageProcessing(std::vector<tp::RabbitClient::Message> mgs);
    void WriteMetricToFile(const tp::Metric& metric);


private:
    size_t workers_;
    std::vector<size_t> worker_pids_;
    tp::RabbitClient client_;
    std::unordered_map<int, tp::Metric> awaiting_metrics_;
    std::vector<ChildProcess> children_;

private:
    inline static bool s_is_running_ = true;
};


