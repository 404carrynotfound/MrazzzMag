#include <iostream>

#include "implementation.h"
#include "interface.h"


int main()
{
    Store* store = createStore();

    auto* const handler = new cli_handler();
    store->setActionHandler(handler);
    // store->setActionHandler(new cli_handler());
    store->init(5, 0, 0);

     std::vector<Client> list;
     list.push_back(Client{ 0, 10, 0, 10 });
     list.push_back(Client{ 45, 35, 0, 30 });
     list.push_back(Client{ 46, 30, 20, 100 });
     list.push_back(Client{ 200, 10, 10, 1 });
//     list.push_back(Client{ 10, 30, 80, 2 });

//    std::vector<Client> list = {
//            Client{0, 300, 0, 300},
//            Client{1, 300, 0, 200},
//            Client{5, 300, 0, 600}
//    };


    store->addClients(list.data(), list.size());
    // store->advanceTo(20);
    // store->advanceTo(30);
    // store->advanceTo(40);
    store->advanceTo(5000);

    std::cout << store->getBanana() << "bs" << store->getSchweppes() << std::endl;
    // handler->print_logs();

    return 0;
}