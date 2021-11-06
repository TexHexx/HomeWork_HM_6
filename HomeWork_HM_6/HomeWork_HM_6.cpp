// HomeWork_HM_6.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <mutex>
#include <thread>
#include <string>
#include <utility>
#include <condition_variable>
#include <queue>

using namespace std;
//firstTask
class pcout {
private:
    mutex coutMutex;

    void _print() {};
    template<class First, class... Rest>
    void _print(First&& first, Rest&& ...rest) {
        cout << std::forward<First>(first);
        if ((sizeof ... (rest)) == 0)
            cout << endl;
        _print(std::forward<Rest>(rest)...);
    };

public:
    void print() { };
    template<class... Args>
    void print(Args&& ...args) {
        lock_guard<mutex> g(coutMutex);
        _print(std::forward<Args>(args)...);
    };
};

void firstTask() {
    pcout printer;
    auto threadPrint = [&printer](const string& threadName) {
        for (size_t i = 0u; i < 10u; ++i) {
            printer.print(threadName, " ", i);
        }
    };

    thread printer1(threadPrint, "printer1");
    thread printer2(threadPrint, "printer2");
    thread printer3(threadPrint, "printer3");
    printer1.join();
    printer2.join();
    printer3.join();
};
//firstTask
//secondTask

bool needToFinish = false;
int foundAmount = 0;

bool numberIsPrime(int number) {
    if (number < 2) return false;
    if (number % 2 == 0) return (number == 2);
    if (number % 3 == 0) return (number == 3);
    if (number % 5 == 0) return (number == 5);
    int root = static_cast<int>(sqrtf(static_cast<float>(number)));
    for (int i = 3; i <= root; i += 2)
    {
        if (number % i == 0) return false;
    }
    return true;
};

void secondTask() {
    int number = 0;

    auto calcker = [&number](int needed) {
        while (foundAmount < needed)
        {
            if (numberIsPrime(++number)) {
                ++foundAmount;
            }
        }
        needToFinish = true;
    };

    auto printer = [&number](int needed) {
        int donePercent = 0;
        cout << "\r" << donePercent << "% completed";
        while (!needToFinish) {
            int newPercent = foundAmount * 100 / needed;
            if (newPercent > donePercent) {
                donePercent = newPercent;
                cout << "\r" << donePercent << "% completed";
            }
        }
        cout << "\r" << "100% completed" << endl;
        cout << "Found number: " << number << endl;
    };

    int needed = 0;
    cout << "Enter number:";
    cout.flush();
    cin >> needed;
    thread calcThread(calcker, needed);
    printer(needed);
    calcThread.join();
};
//secondTask
//thirdTask

condition_variable sync;
mutex syncMutex;
pcout printer;

void thirdTask() {
    priority_queue<int> goods;
    srand(time(NULL));

    cout << "Initital queue: ";
    for (size_t i = 0u; i < 20; ++i) {
        int value = rand();
        goods.push(value);
        cout << value << " ";
    }
    cout << endl;

    auto consumer = [&goods]() {
        unique_lock<mutex> ul(syncMutex);
        for (size_t i = 0u; i < 100u; ++i) {
            sync.wait(ul, [&goods]() { return !goods.empty(); });
            cout << "Grabbed: " << goods.top() << endl;
            goods.pop();
        }
    };

    auto producer = [&goods]() {
        lock_guard<mutex> lg(syncMutex);
        for (size_t i = 0u; i < 100u; ++i) {
            int good = rand();
            goods.push(good);
            cout << "Bring: " << good << endl;
            sync.notify_one();
        }
    };

    thread consumerThread(consumer);
    thread producerThread(producer);
    consumerThread.join();
    producerThread.join();
    
    cout << "Result queue: ";
    for (size_t i = 0u; i < goods.size(); ++i) {
        cout << goods.top() << " ";
        goods.pop();
    }
    cout << endl;
};
//thirdTask
int main()
{
    firstTask();
    secondTask();
    thirdTask();
}
