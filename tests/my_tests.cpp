#define CATCH_CONFIG_MAIN

#include "../interface.h"
#include "catch.hpp"

#include <vector>

const int RESTOCK_TIME = 60;
const int RESTOCK_AMOUNT = 100;

typedef std::vector<Client> ClientList;

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
};

struct TestStore : ActionHandler {
    Store *impl = nullptr;
    std::vector<StoreEvent> log;

    TestStore()
            : impl(createStore()) {
        impl->setActionHandler(this);
    }

    ~TestStore() {
        delete impl;
    }

    TestStore(const TestStore &) = delete;

    TestStore &operator=(const TestStore &) = delete;

    void init(int workerCount, int startBanana, int startSchweppes) {
        impl->init(workerCount, startBanana, startSchweppes);
    }

    void advanceTo(int minute) {
        impl->advanceTo(minute);
    }

    int getBanana() const {
        return impl->getBanana();
    }

    int getSchweppes() const {
        return impl->getSchweppes();
    }

    void addClients(const ClientList &clients) {
        impl->addClients(clients.data(), clients.size());
    }

    void addClients(const Client &single) {
        impl->addClients(&single, 1);
    }

    void onWorkerSend(int minute, ResourceType resource) override {
        StoreEvent ev{};
        ev.type = StoreEvent::WorkerSend;
        ev.minute = minute;
        ev.worker.resource = resource;
        log.push_back(ev);
    }

    void onWorkerBack(int minute, ResourceType resource) override {
        StoreEvent ev{};
        ev.type = StoreEvent::WorkerBack;
        ev.minute = minute;
        ev.worker.resource = resource;
        log.push_back(ev);
    }

    void onClientDepart(int index, int minute, int banana, int schweppes) override {
        StoreEvent ev{};
        ev.type = StoreEvent::ClientDepart;
        ev.minute = minute;
        ev.client.banana = banana;
        ev.client.schweppes = schweppes;
        ev.client.index = index;
        log.push_back(ev);
    }
};

#define LastEvent() (store.log.back())

TEST_CASE("Two clients depart at the same time")
{
    TestStore store;
    store.init(0,10,10);
    store.addClients({
                             Client{10, 15, 10, 16},
                             Client{25, 15,100, 1}
                     });
    store.advanceTo(26);

    REQUIRE(store.log.size() == 2);
    REQUIRE(store.log[0].type == StoreEvent::ClientDepart);
    REQUIRE(store.log[0].client.banana == 10);
    REQUIRE(store.log[0].client.schweppes == 10);

    REQUIRE(store.log[1].type == StoreEvent::ClientDepart);
    REQUIRE(store.log[1].client.banana == 0);
    REQUIRE(store.log[1].client.schweppes == 0);
}

TEST_CASE("Restoring banana stocks is a priority")
{
    TestStore store;
    store.init(3, 0, 0);
    store.addClients({
        Client{10, 230, 20, 90},
        Client{20, 15, 25, 30},
        Client{30, 20, 20, 10},
        Client{100, 30, 20, 20}
    });

    SECTION("Three events for restocking bananas")
    {
        store.advanceTo(10);

        REQUIRE(store.log.size() == 3);

        REQUIRE(store.log[0].type == StoreEvent::WorkerSend);
        REQUIRE(store.log[0].worker.resource == ResourceType::banana);
        REQUIRE(store.log[0].minute == 10);

        REQUIRE(store.log[1].type == StoreEvent::WorkerSend);
        REQUIRE(store.log[1].worker.resource == ResourceType::banana);
        REQUIRE(store.log[1].minute == 10);

        REQUIRE(store.log[2].type == StoreEvent::WorkerSend);
        REQUIRE(store.log[2].worker.resource == ResourceType::banana);
        REQUIRE(store.log[2].minute == 10);
    }

    SECTION("Second client must not trigger workers")
    {
        store.advanceTo(20);

        REQUIRE(store.log.size() == 3);

        REQUIRE(LastEvent().type == StoreEvent::WorkerSend);
        REQUIRE(LastEvent().worker.resource == ResourceType::banana);
        REQUIRE(LastEvent().minute == 10);
    }

    SECTION("Third client must leave before second")
    {
        store.advanceTo(50);

        REQUIRE(store.log.size() == 5);

        REQUIRE(store.log[3].type == StoreEvent::ClientDepart);
        REQUIRE(store.log[3].minute == 40);
        REQUIRE(store.log[3].client.banana == 0);
        REQUIRE(store.log[3].client.schweppes == 0);
        REQUIRE(store.log[3].client.index == 2);

        REQUIRE(store.log[4].type == StoreEvent::ClientDepart);
        REQUIRE(store.log[4].minute == 50);
        REQUIRE(store.log[4].client.banana == 0);
        REQUIRE(store.log[4].client.schweppes == 0);
        REQUIRE(store.log[4].client.index == 1);
    }

    SECTION("Last event must be at 160 minute")
    {
        store.advanceTo(200);

        REQUIRE(store.log.size() == 12);

        REQUIRE(store.log[10].type == StoreEvent::ClientDepart);
        REQUIRE(store.log[10].minute == 120);
        REQUIRE(store.log[10].client.banana == 30);
        REQUIRE(store.log[10].client.schweppes == 0);
        REQUIRE(store.log[10].client.index == 3);

        REQUIRE(LastEvent().minute == 160);
        REQUIRE(LastEvent().type == StoreEvent::WorkerBack);
        REQUIRE(LastEvent().worker.resource == ResourceType::schweppes);


    }

    SECTION("Store stock")
    {
        store.advanceTo(200);

        REQUIRE(store.log.size() == 12);

        REQUIRE(store.getBanana() == 40);
        REQUIRE(store.getSchweppes() == 100);
    }
}

TEST_CASE("Clients just watching in the store")
{
    TestStore store;
    store.init(1, 0, 0);
    store.addClients({
        Client{0, 0, 0, 90},
        Client{90, 0, 0, 30}
    });

    SECTION("Clients must not trigger workers")
    {
        store.advanceTo(0);
        REQUIRE(store.log.size() == 1);

        REQUIRE(LastEvent().type == StoreEvent::ClientDepart);
        REQUIRE(LastEvent().minute == 0);
        REQUIRE(LastEvent().client.index == 0);
        REQUIRE(LastEvent().client.banana == 0);
        REQUIRE(LastEvent().client.schweppes == 0);
    }

    SECTION("First client must leave before second")
    {
        store.advanceTo(90);

        REQUIRE(store.log.size() == 2);

        REQUIRE(LastEvent().type == StoreEvent::ClientDepart);
        REQUIRE(LastEvent().minute == 90);
        REQUIRE(LastEvent().client.index == 1);
        REQUIRE(LastEvent().client.banana == 0);
        REQUIRE(LastEvent().client.schweppes == 0);

        REQUIRE(store.getSchweppes() == 0);
        REQUIRE(store.getBanana() == 0);
    }

}