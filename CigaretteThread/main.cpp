#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <random>
#include <chrono>
#include <map>

using namespace std;

// Sémaphores pour les fumeurs
binary_semaphore butterGreedyChildrenSem(0);
binary_semaphore breadGreedyChildrenSem(0);
binary_semaphore jamGreedyChildrenSem(0);
binary_semaphore motherSem(1); // Sémaphore pour synchroniser l'mother

// Mutex pour protéger l'accès aux ressources partagées
mutex mtx;
bool isButter = false, isBread = false, isJam = false;

// Map pour afficher les ingrédients
map<unsigned, string> ingredients = { {0, "Butter"}, {1, "Bread"}, {2, "Jam"} };

// Fumeur avec tabac
void greedyChildrenWithButter() {
    while (true) {
        butterGreedyChildrenSem.acquire(); // Attend que le médiateur réveille ce fumeur
        cout << "Le fumeur avec le tabac fabrique une cigarette.\n";
        this_thread::sleep_for(chrono::seconds(1));  // Simule la fabrication d'une cigarette
        cout << "Le fumeur avec le tabac fume une cigarette.\n";
        motherSem.release();  // Réveille l'mother pour qu'il fournisse les prochains ingrédients
    }
}

// Fumeur avec papier
void greedyChildrenWithBread() {
    while (true) {
        breadGreedyChildrenSem.acquire();  // Attend que le médiateur réveille ce fumeur
        cout << "Le fumeur avec le papier fabrique une cigarette.\n";
        this_thread::sleep_for(chrono::seconds(1));  // Simule la fabrication d'une cigarette
        cout << "Le fumeur avec le papier fume une cigarette.\n";
        motherSem.release();  // Réveille l'mother pour qu'il fournisse les prochains ingrédients
    }
}

// Fumeur avec allumettes
void greedyChildrenWithJam() {
    while (true) {
        jamGreedyChildrenSem.acquire();  // Attend que le médiateur réveille ce fumeur
        cout << "Le fumeur avec les allumettes fabrique une cigarette.\n";
        this_thread::sleep_for(chrono::seconds(1));  // Simule la fabrication d'une cigarette
        cout << "Le fumeur avec les allumettes fume une cigarette.\n";
        motherSem.release();  // Réveille l'mother pour qu'il fournisse les prochains ingrédients
    }
}

// Médiateur
void mediator() {
    while (true) {
        lock_guard<mutex> lock(mtx);

        if (isButter && isBread) {
            cout << "Réveille le fumeur avec allumettes\n";
            jamGreedyChildrenSem.release();
            isButter = isBread = false;
        } else if (isButter && isJam) {
            cout << "Réveille le fumeur avec papier\n";
            breadGreedyChildrenSem.release();
            isButter = isJam = false;
        } else if (isBread && isJam) {
            cout << "Réveille le fumeur avec tabac\n";
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
            cout << "Mother place les ingrédients : " << ingredients[ingredient1] << " et " << ingredients[ingredient2] << endl;
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
    // Démarrer les threads
    thread motherThread(mother);
    thread mediatorThread(mediator);
    thread greedyChildrenWithButterThread(greedyChildrenWithButter);
    thread greedyChildrenWithBreadThread(greedyChildrenWithBread);
    thread greedyChildrenWithJamThread(greedyChildrenWithJam);

    // Joindre les threads
    motherThread.join();
    mediatorThread.join();
    greedyChildrenWithButterThread.join();
    greedyChildrenWithBreadThread.join();
    greedyChildrenWithJamThread.join();

    return 0;
}
