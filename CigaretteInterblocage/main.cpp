#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <random>
#include <chrono>
#include <map>

using namespace std;

// Semaphores for ingredients and synchronization
binary_semaphore breadSem(0);  // Bread (tartine)
binary_semaphore butterSem(0); // Butter
binary_semaphore jamSem(0);    // Jam
binary_semaphore motherSem(1); // Semaphore for the mother (agent)

// Mutex for protecting shared resources
mutex mtx;

// Map to display ingredients
map<unsigned, string> ingredients = {
    {0, "Bread"},
    {1, "Butter"},
    {2, "Jam"}
};

//---------------------------//
//          Children         //
//---------------------------//

// Child with bread (needs butter and jam)
void childWithBread() {
    while (true) {
        {
            lock_guard<mutex> lock(mtx);  // Ensures atomic acquisition of semaphores
            butterSem.acquire();
            jamSem.acquire();
        }
        cout << "The child with bread is preparing breakfast.\n";
        this_thread::sleep_for(chrono::seconds(1));
        motherSem.release();
        cout << "The child with bread is eating breakfast.\n";
    }
}

// Child with butter (needs bread and jam)
void childWithButter() {
    while (true) {
        {
            lock_guard<mutex> lock(mtx);  // Ensures atomic acquisition of semaphores
            breadSem.acquire();
            jamSem.acquire();
        }
        cout << "The child with butter is preparing breakfast.\n";
        this_thread::sleep_for(chrono::seconds(1));
        motherSem.release();
        cout << "The child with butter is eating breakfast.\n";
    }
}

// Child with jam (needs bread and butter)
void childWithJam() {
    while (true) {
        {
            lock_guard<mutex> lock(mtx);  // Ensures atomic acquisition of semaphores
            breadSem.acquire();
            butterSem.acquire();
        }
        cout << "The child with jam is preparing breakfast.\n";
        this_thread::sleep_for(chrono::seconds(1));
        motherSem.release();
        cout << "The child with jam is eating breakfast.\n";
    }
}

// The mother who distributes ingredients
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

        cout << "The mother places the ingredients: " << ingredients[ingredient1] << " and " << ingredients[ingredient2] << endl;

        if ((ingredient1 == 0 && ingredient2 == 1) || (ingredient1 == 1 && ingredient2 == 0)) {
            breadSem.release();
            butterSem.release();
        } else if ((ingredient1 == 0 && ingredient2 == 2) || (ingredient1 == 2 && ingredient2 == 0)) {
            breadSem.release();
            jamSem.release();
        } else if ((ingredient1 == 1 && ingredient2 == 2) || (ingredient1 == 2 && ingredient1 == 1)) {
            butterSem.release();
            jamSem.release();
        }
    }
}

int main() {
    // Start threads
    thread motherThread(mother);
    thread childWithBreadThread(childWithBread);
    thread childWithButterThread(childWithButter);
    thread childWithJamThread(childWithJam);

    // Join threads
    motherThread.join();
    childWithBreadThread.join();
    childWithButterThread.join();
    childWithJamThread.join();

    return 0;
}
