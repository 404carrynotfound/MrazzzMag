#pragma once

#include <iostream>
#include <cassert>
#include <cmath>

#include "./queue/queue.h"

#include "interface.h"

#define RESTOCK_TIME 60
#define RESTOCK_QUANTITY 100

/// This is sample empty implementation you can place your solution here or delete this and include tests to your solution

struct indexed_client : Client
{
    int index;
    indexed_client() = default;
    indexed_client(int index, Client client) : Client{ client } { this->index = index; }
};

struct Worker
{
    ResourceType type = {};
    int arrival = {};
};

using clients_list = Queue<indexed_client>;
using workers_list = Queue<Worker>;

struct MyStore : Store {
    ActionHandler* actionHandler = nullptr;

    void setActionHandler(ActionHandler* handler) override;

    void init(int workerCount, int startBanana, int startSchweppes) override;

    void addClients(const Client* clients, int count) override;

    void advanceTo(int minute) override;

    int getBanana() const override;

    int getSchweppes() const override;

private:
    int time_ = 0;

    int client_index_ = 0;

    clients_list clients_ = {};
    clients_list ordered_clients = {};

    int banana_ = 0;
    int schweppes_ = 0;
    int workers_ = {};

    int waiting_bananas_ = { 0 };
    int waiting_schweppes_ = { 0 };

    workers_list workers_banana_ = {};
    workers_list workers_schweppes_ = {};

    void serve();

    void restock_banana(int quantity, int arrive);

    void restock_schweppes(int quantity, int arrive);

    void check_delivery();
};

struct cli_handler : ActionHandler
{
    void onWorkerSend(int minute, ResourceType resource) override;

    void onWorkerBack(int minute, ResourceType resource) override;

    void onClientDepart(int index, int minute, int banana, int schweppes) override;
};