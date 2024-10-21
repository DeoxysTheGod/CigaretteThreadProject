#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <random>
#include <chrono>
#include <map>

using namespace std;

// Semaphores for greedy children
binary_semaphore butterGreedyChildrenSem(0);
binary_semaphore breadGreedyChildrenSem(0);
binary_semaphore jamGreedyChildrenSem(0);
binary_semaphore motherSem(1); // Semaphore to synchronize the mother

// Mutex to protect access to shared resources
mutex mtx;
bool isButter = false, isBread = false, isJam = false;

// Map to display ingredients
map<unsigned, string> ingredients = { {0, "Butter"}, {1, "Bread"}, {2, "Jam"} };

// Greedy child with butter
void greedyChildrenWithButter() {
    while (true) {
        butterGreedyChildrenSem.acquire(); // Waits for the mediator to wake up this child
        cout << "The greedy child with butter is preparing breakfast.\n";
        this_thread::sleep_for(chrono::seconds(1));  // Simulates preparing breakfast
        cout << "The greedy child with butter is eating breakfast.\n";
        motherSem.release();  // Wakes up the mother to provide the next ingredients
    }
}

// Greedy child with bread
void greedyChildrenWithBread() {
    while (true) {
        breadGreedyChildrenSem.acquire();  // Waits for the mediator to wake up this child
        cout << "The greedy child with bread is preparing breakfast.\n";
        this_thread::sleep_for(chrono::seconds(1));  // Simulates preparing breakfast
        cout << "The greedy child with bread is eating breakfast.\n";
        motherSem.release();  // Wakes up the mother to provide the next ingredients
    }
}

// Greedy child with jam
void greedyChildrenWithJam() {
    while (true) {
        jamGreedyChildrenSem.acquire();  // Waits for the mediator to wake up this child
        cout << "The greedy child with jam is preparing breakfast.\n";
        this_thread::sleep_for(chrono::seconds(1));  // Simulates preparing breakfast
        cout << "The greedy child with jam is eating breakfast.\n";
        motherSem.release();  // Wakes up the mother to provide the next ingredients
    }
}

// Mediator
void mediator() {
    while (true) {
        lock_guard<mutex> lock(mtx);

        if (isButter && isBread) {
            cout << "Wakes up the greedy child with jam\n";
            jamGreedyChildrenSem.release();
            isButter = isBread = false;
        } else if (isButter && isJam) {
            cout << "Wakes up the greedy child with bread\n";
            breadGreedyChildrenSem.release();
            isButter = isJam = false;
        } else if (isBread && isJam) {
            cout << "Wakes up the greedy child with butter\n";
            butterGreedyChildrenSem.release();
            isBread = isJam = false;
        }
    }
}

// Mother
void mother() {
    while (true) {
        motherSem.acquire();
        this_thread::sleep_for(chrono::seconds(1));
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(0, 2);
        int ingredient1 = dist(gen);
        int ingredient2 = dist(gen);
        while (ingredient1 == ingredient2) {
            ingredient2 = dist(gen);
        }
        {
            lock_guard<mutex> lock(mtx);
            cout << "Mother places the ingredients: " << ingredients[ingredient1] << " and " << ingredients[ingredient2] << endl;
            if ((ingredient1 == 0 && ingredient2 == 1) || (ingredient1 == 1 && ingredient2 == 0)) {
                isButter = true;
                isBread = true;
            } else if ((ingredient1 == 0 && ingredient2 == 2) || (ingredient1 == 2 && ingredient2 == 0)) {
                isButter = true;
                isJam = true;
            } else if ((ingredient1 == 1 && ingredient2 == 2) || (ingredient1 == 2 && ingredient1 == 1)) {
                isBread = true;
                isJam = true;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}


int main() {
    // Start threads
    thread motherThread(mother);
    thread mediatorThread(mediator);
    thread greedyChildrenWithButterThread(greedyChildrenWithButter);
    thread greedyChildrenWithBreadThread(greedyChildrenWithBread);
    thread greedyChildrenWithJamThread(greedyChildrenWithJam);

    // Join threads
    motherThread.join();
    mediatorThread.join();
    greedyChildrenWithButterThread.join();
    greedyChildrenWithBreadThread.join();
    greedyChildrenWithJamThread.join();

    return 0;
}
