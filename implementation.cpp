#include "implementation.h"

void MyStore::setActionHandler(ActionHandler *handler)
{
    actionHandler = handler;
}

void MyStore::init(int workerCount, int startBanana, int startSchweppes)
{
    workers_ = workerCount;
    banana_ = startBanana;
    schweppes_ = startSchweppes;
}

void MyStore::addClients(const Client *clients, int count)
{
    assert(clients && count != 0);
	for (int i = 0; i < count; ++i)
    {
        clients_.push(indexed_client{client_index_++, clients[i]});
    }
}

void MyStore::advanceTo(int minute)
{
    while (time_ <= minute)
    {
        serve();
        check_delivery();
        time_++;
    }
}

int MyStore::getBanana() const
{
    return banana_;
}

int MyStore::getSchweppes() const
{
    return schweppes_;
}

void MyStore::serve()
{

    for (int i = 0; i < clients_.size(); ++i)
    {
        const indexed_client current = clients_.front();

        /// Проверяваме  клиентите дали са пристигнали
        if (current.arriveMinute <= time_)
        {
            /// Проверяваме дали има налични банани и швепс ги даваме на клиента след което той си тръгва.
            if (current.banana <= banana_ && current.schweppes <= schweppes_)
            {
                actionHandler->onClientDepart(current.index, current.arriveMinute, current.banana, current.schweppes);
                banana_ -= current.banana;
                schweppes_ -= current.schweppes;
            }
            /// Ако имаме достъчно банани, но нямаме швепс подаваме на желаните количества швепс на функция отговорна за зареждането.
            else if (current.banana <= banana_)
            {
                if (waiting_schweppes_ < current.schweppes)
                {
                    restock_schweppes(current.schweppes, current.arriveMinute);
                }
                /// Добавяме клиента в опашка за чакане.
                ordered_clients.push(current);
            }
            /// Ако имаме достъчно швепс, но нямаме банани подаваме на желаните количества банани на функция отговорна за зареждането.
            else if (current.schweppes <= schweppes_)
            {
                if (waiting_bananas_ < current.banana)
                {
                    restock_banana(current.banana, current.arriveMinute);
                }
                /// Добавяме клиента в опашка за чакане.
                ordered_clients.push(current);
            }
            /// Ако нямаме банани и швепс, зареждаме банани и швепс, ако бананите са с приоритет ако са равни.
            else
            {
                /// Ако имаме един работник тогава проверяваме дали ни трябва повече швепс.
                if (current.banana < current.schweppes && workers_ == 1)
                {
                    restock_schweppes(current.schweppes, current.arriveMinute);
                }
                else
                {
                    restock_banana(current.banana, current.arriveMinute);
                    restock_schweppes(current.schweppes, current.arriveMinute);
                }
                /// Добавяме клиента в опашка за чакане.
                ordered_clients.push(current);
            }
            /// Премахваме клиетна който е обслужен или чака
            clients_.pop();
            /// Намаляваме индекса, защото след като сме премахнали клиента размера на опашката се намалява.
            --i;
        }
    }

    /// Проверяваме за доставка на продукти.
    check_delivery();

    for (int i = 0; i < ordered_clients.size(); ++i)
    {
        const indexed_client current = ordered_clients.front();

        /// Проверяваме дали има налични банани и швепс ги даваме на текущия клиент.
        if (current.banana <= banana_ && current.schweppes <= schweppes_)
        {
            actionHandler->onClientDepart(current.index, time_, current.banana, current.schweppes);
            banana_ -= current.banana;
            schweppes_ -= current.schweppes;
            ordered_clients.pop();
            --i;
        }
        /// Ако времето му за чакане е свършило, тогава му даваме количество което се нуждае, но ако нямаме му даваме колкото имаме налични.
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
            ordered_clients.pop();
            --i;
        }
        /// Ако няма нужните продукти и има още време да чака го преместваме в края на опашката.
        else
        {
            ordered_clients.push(current);
            ordered_clients.pop();
        }
    }
}

/// Приемаме количеството банани което ни трябва и минутата на пристигане.
void MyStore::restock_banana(int quantity, int arrive)
{
    /// Ако нямаме свободни работници или чакаме повече банани прекратяване поръчката.
    if (workers_ == 0 || quantity <= waiting_bananas_) return;
    /// Намираме колко работника са ни нужни за поръчката да бъде изпълнена.
    const int workers = static_cast<int>(ceil(abs(waiting_bananas_ - quantity) / static_cast<double>(RESTOCK_QUANTITY)));

    int on_duty = { 0 };

    for (int i = 0; i < workers && workers_ != 0; ++i)
    {
        workers_banana_.push(Worker{ ResourceType::banana, arrive + RESTOCK_TIME });
        actionHandler->onWorkerSend(arrive, ResourceType::banana);
        workers_--;
        on_duty++;
    }
    /// Запазваме поръчаното количесто банани като използваме колко работника са изпратени за зареждане.
    waiting_bananas_ += on_duty * RESTOCK_QUANTITY;
}

/// Приемаме количеството швепс което ни трябва и минутата на пристигане.
void MyStore::restock_schweppes(int quantity, int arrive)
{
    /// Ако нямаме свободни работници или чакаме повече швепс прекратяване поръчката.
    if (workers_ == 0 || quantity <= waiting_schweppes_) return;
    /// Намираме колко работника са ни нужни за поръчката да бъде изпълнена.
    const int workers = static_cast<int>(ceil(abs(waiting_schweppes_ - quantity) / static_cast<double>(RESTOCK_QUANTITY)));

    int on_duty = { 0 };

    for (int i = 0; i < workers && workers_ != 0; ++i)
    {
        workers_schweppes_.push(Worker{ ResourceType::schweppes, arrive + RESTOCK_TIME });
        actionHandler->onWorkerSend(arrive, ResourceType::schweppes);
        workers_--;
        on_duty++;
    }
    /// Запазваме поръчаното количесто швепс като използваме колко работника са изпратени за зареждане.
    waiting_schweppes_ += on_duty * RESTOCK_QUANTITY;
}

void MyStore::check_delivery()
{
    /// Проверяваме дали чакаме банани.
    if (waiting_bananas_ != 0)
    {
        /// Проверяваме докато работниците изпратени за банани не се върнат или времето на пристигане е повече от текущото.
        while (!workers_banana_.empty() && workers_banana_.front().arrival <= time_)
        {
            const Worker current = workers_banana_.front();
            workers_banana_.pop();

            banana_ += RESTOCK_QUANTITY;
            waiting_bananas_ -= RESTOCK_QUANTITY;

            workers_++;

            actionHandler->onWorkerBack(current.arrival, ResourceType::banana);
        }
    }
    /// Проверяваме дали чакаме швепс.
    if (waiting_schweppes_ != 0)
    {
        /// Проверяваме докато работниците изпратени за швепс не се върнат или времето на пристигане е повече от текущото.
        while (!workers_schweppes_.empty() && workers_schweppes_.front().arrival <= time_)
        {
            const Worker current = workers_schweppes_.front();
            workers_schweppes_.pop();

            schweppes_ += RESTOCK_QUANTITY;
            waiting_schweppes_ -= RESTOCK_QUANTITY;

            workers_++;

            actionHandler->onWorkerBack(current.arrival, ResourceType::schweppes);
        }
    }
}

void cli_handler::onWorkerSend(int minute, ResourceType resource)
{
    std::cout << "W " << minute;
    if (resource == ResourceType::banana)
        std::cout << " banana";
    else
        std::cout << " schweppes";
    std::cout << std::endl;
}

void cli_handler::onWorkerBack(int minute, ResourceType resource)
{
    std::cout << "D " << minute;
    if (resource == ResourceType::banana)
        std::cout << " banana";
    else
        std::cout << " schweppes";
    std::cout << std::endl;
}

void cli_handler::onClientDepart(int index, int minute, int banana, int schweppes)
{
    std::cout << index << " " << minute << " "
              << banana << " " << schweppes << std::endl;
}

Store* createStore()
{
    return new MyStore();
}