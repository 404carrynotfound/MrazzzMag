#pragma once
#include <iostream>
#include <cassert>
#include <queue>
#include <vector>
#include <cmath>

#include "interface.h"

#define RESTOCK_TIME 60
#define RESTOCK_QUANTITY 100

/// This is sample empty implementation you can place your solution here or delete this and include tests to your solution

struct indexed_client : Client
{
    int index;
    indexed_client(int index, Client client) : Client{ client } { this->index = index; }

    bool operator<(const indexed_client& other) const
    {
        if (maxWaitTime + arriveMinute == other.maxWaitTime + other.arriveMinute)
        {
            return index < other.index;
        }
        return maxWaitTime + arriveMinute < other.maxWaitTime + other.arriveMinute;
    }

    bool operator>(const indexed_client& other) const
    {
        if (maxWaitTime + arriveMinute == other.maxWaitTime + other.arriveMinute)
        {
            return index > other.index;
        }
        return maxWaitTime + arriveMinute > other.maxWaitTime + other.arriveMinute;
    }
};

struct Worker
{
    ResourceType type = {};
    int arrival = {};
};

using clients_list = std::queue<indexed_client>;
using workers_list = std::queue<Worker>;

using waiting_clients = std::priority_queue<indexed_client, std::vector<indexed_client>, std::greater<>>;

struct MyStore : Store {
    ActionHandler* actionHandler = nullptr;

    void setActionHandler(ActionHandler* handler) override
    {
        actionHandler = handler;
    }

    void init(int workerCount, int startBanana, int startSchweppes) override
    {
        workers_ = workerCount;
        banana_ = startBanana;
        schweppes_ = startSchweppes;
    }

    void addClients(const Client* clients, int count) override
    {
        assert(clients && count != 0);
        for (int i = 0; i < count; ++i)
        {
            clients_.push(indexed_client{i, clients[i]});
        }
        clients_count_ = count + 1;
    }

    void advanceTo(int minute) override
    {
        while (time_ <= minute)
        {
            attendance();
            check_delivery();
            time_++;
        }
    }

    int getBanana() const override
    {
        return banana_;
    }

    int getSchweppes() const override
    {
        return schweppes_;
    }

    void attendance()
    {
        int counter = {};
        while (!clients_.empty() && counter != clients_count_ + 1)
        {
            const auto& current = clients_.front();

            if (current.arriveMinute <= time_)
            {
                if (current.banana <= banana_ && current.schweppes <= schweppes_)
                {
                    actionHandler->onClientDepart(current.index, time_, current.banana, current.schweppes);
                    banana_ -= current.banana;
                    schweppes_ -= current.schweppes;
                }
                else if (current.banana <= banana_)
                {
                    if (waiting_schweppes_ < current.schweppes)
                    {
                        restock_schweppes(current.schweppes, current.arriveMinute);
                    }
                    waiting_clients_.push(current);
                    waiting_clients_count_++;
                }
                else if (current.schweppes <= schweppes_)
                {
                    if (waiting_bananas_ < current.banana)
                    {
                        restock_banana(current.banana, current.arriveMinute);
                    }
                    waiting_clients_.push(current);
                    waiting_clients_count_++;
                }
                else
                {
                    restock_banana(current.banana, current.arriveMinute);
                    restock_schweppes(current.schweppes, current.arriveMinute);
                    waiting_clients_.push(current);
                    waiting_clients_count_++;
                }
                clients_.pop();
                clients_count_--;
            }
            check_delivery();
            counter++;
        }

        counter = 0;

        while (!waiting_clients_.empty() && counter != waiting_clients_count_ + 1)
        {
            const auto &current = waiting_clients_.top();

            if (current.banana <= banana_ && current.schweppes <= schweppes_)
            {
                actionHandler->onClientDepart(current.index, time_, current.banana, current.schweppes);
                banana_ -= current.banana;
                schweppes_ -= current.schweppes;
                waiting_clients_.pop();
                waiting_clients_count_--;
            }
            else if (current.arriveMinute + current.maxWaitTime <= time_)
            {
                if (current.banana <= banana_ && current.schweppes <= schweppes_)
                {
                    banana_ -= current.banana;
                    schweppes_ -= current.schweppes;
                    actionHandler->onClientDepart(current.index, current.arriveMinute + current.maxWaitTime, current.banana, current.schweppes);
                }
                else if (current.banana <= banana_) {
                    banana_ -= current.banana;
                    actionHandler->onClientDepart(current.index, current.arriveMinute + current.maxWaitTime, current.banana, schweppes_);
                    schweppes_ = 0;
                }
                else if (current.schweppes <= schweppes_) {
                    schweppes_ -= current.schweppes;
                    actionHandler->onClientDepart(current.index, current.arriveMinute + current.maxWaitTime, banana_, current.schweppes);
                    banana_ = 0;
                }
                else {
                    actionHandler->onClientDepart(current.index, current.arriveMinute + current.maxWaitTime, banana_, schweppes_);
                    banana_ = 0;
                    schweppes_ = 0;
                }
                waiting_clients_.pop();
                waiting_clients_count_--;
            }
            check_delivery();
            counter++;
        }
    }

private:
    int time_ = { 0 };

    int clients_count_ = {};
    int waiting_clients_count_ = {};

    clients_list clients_ = {};
    int banana_ = { 0 };
    int schweppes_ = { 0 };
    int workers_ = {};

    int waiting_bananas_ = { 0 };
    int waiting_schweppes_ = { 0 };

    workers_list workers_banana_ = {};
    workers_list workers_schweppes_ = {};

    waiting_clients waiting_clients_ = {};

    void restock_banana(int quantity, int arrive)
    {
        if (workers_ == 0 || quantity <= waiting_bananas_) return;

        const auto workers = static_cast<int>(ceil(abs(waiting_bananas_ - quantity) / static_cast<double>(RESTOCK_QUANTITY)));

        int on_duty = { 0 };

        for (int i = 0; i < workers && workers_ != 0; ++i)
        {
            workers_banana_.push(Worker{ ResourceType::banana, arrive + RESTOCK_TIME });
            actionHandler->onWorkerSend(arrive, ResourceType::banana);
            workers_--;
            on_duty++;
        }
        waiting_bananas_ += on_duty * RESTOCK_QUANTITY;
    }

    void restock_schweppes(int quantity, int arrive)
    {
        if (workers_ == 0 || quantity <= waiting_schweppes_) return;

        const auto workers = static_cast<int>(ceil(abs(waiting_schweppes_ - quantity) / static_cast<double>(RESTOCK_QUANTITY)));

        int on_duty = { 0 };

        for (int i = 0; i < workers && workers_ != 0; ++i)
        {
            workers_schweppes_.push(Worker{ ResourceType::schweppes, arrive + RESTOCK_TIME });
            actionHandler->onWorkerSend(arrive, ResourceType::schweppes);
            workers_--;
            on_duty++;
        }
        waiting_schweppes_ += on_duty * RESTOCK_QUANTITY;
    }

    void check_delivery()
    {
        if (waiting_bananas_ != 0)
        {
            while (!workers_banana_.empty() && workers_banana_.front().arrival <= time_)
            {
                const auto& current = workers_banana_.front();
                workers_banana_.pop();

                banana_ += RESTOCK_QUANTITY;
                waiting_bananas_ -= RESTOCK_QUANTITY;

                actionHandler->onWorkerBack(current.arrival, ResourceType::banana);
            }
        }

        if (waiting_schweppes_ != 0)
        {
            while (!workers_schweppes_.empty() && workers_schweppes_.front().arrival <= time_)
            {
                const auto& current = workers_schweppes_.front();
                workers_schweppes_.pop();

                schweppes_ += RESTOCK_QUANTITY;
                waiting_schweppes_ -= RESTOCK_QUANTITY;

                actionHandler->onWorkerBack(current.arrival, ResourceType::schweppes);
            }
        }
    }
};

Store* createStore() {
    return new MyStore();
}

struct StoreEvent {
    enum Type {
        WorkerSend, WorkerBack, ClientDepart
    };

    Type type;
    int minute;

    struct Worker {
        ResourceType resource;
    } worker;

    struct Client {
        int index;
        int banana;
        int schweppes;
    } client;

    bool operator>(const StoreEvent& other) const
    {
        return minute > other.minute;
    }
};

struct cli_handler : ActionHandler
{
    void onWorkerSend(int minute, ResourceType resource) override
    {
        std::cout << "W " << minute;
        if (resource == ResourceType::banana)
            std::cout << " banana";
        else
            std::cout << " schweppes";
        std::cout << std::endl;
    }

    void onWorkerBack(int minute, ResourceType resource) override
    {
        std::cout << "D " << minute;
        if (resource == ResourceType::banana)
            std::cout << " banana";
        else
            std::cout << " schweppes";
        std::cout << std::endl;
    }

    void onClientDepart(int index, int minute, int banana, int schweppes) override
    {
        std::cout << index << " " << minute << " "
                  << banana << " " << schweppes << std::endl;
    }
};