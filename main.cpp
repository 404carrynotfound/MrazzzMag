#include <iostream>

#include "implementation.h"

int main()
{
    Store *store = createStore();
    auto* handler = new cli_handler();
    store->setActionHandler(handler);

    int workers = {}, clients_count = {}, working_hours = {};

    std::cin >> workers >> clients_count;

    store->init(workers, 0, 0);

    auto *clients = new Client[clients_count];

    for (int i = 0; i < clients_count; ++i)
    {
        Client current = {};
        std::cin >> current.arriveMinute >> current.banana
                 >> current.schweppes >> current.maxWaitTime;
        working_hours = std::max(current.arriveMinute + current.maxWaitTime, working_hours);
        clients[i] = current;
    }
    store->addClients(clients, clients_count);
    store->advanceTo(working_hours + 60);

    delete[] clients;
    delete handler;
    delete store;

    return 0;
}