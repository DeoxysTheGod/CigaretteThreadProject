#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <random>
#include <chrono>
#include <map>

using namespace std;

// Sémaphores pour les fumeurs
binary_semaphore tobaccoSmokerSem(0);
binary_semaphore paperSmokerSem(0);
binary_semaphore matchSmokerSem(0);
binary_semaphore agentSem(1); // Sémaphore pour synchroniser l'agent

// Mutex pour protéger l'accès aux ressources partagées
mutex mtx;
bool isTobacco = false, isPaper = false, isMatch = false;

// Map pour afficher les ingrédients
map<unsigned, string> ingredients = { {0, "Tobacco"}, {1, "Paper"}, {2, "Match"} };

// Fumeur avec tabac
void smokerWithTobacco() {
    while (true) {
        tobaccoSmokerSem.acquire(); // Attend que le médiateur réveille ce fumeur
        cout << "Le fumeur avec le tabac fabrique une cigarette.\n";
        this_thread::sleep_for(chrono::seconds(1));  // Simule la fabrication d'une cigarette
        cout << "Le fumeur avec le tabac fume une cigarette.\n";
        agentSem.release();  // Réveille l'agent pour qu'il fournisse les prochains ingrédients
    }
}

// Fumeur avec papier
void smokerWithPaper() {
    while (true) {
        paperSmokerSem.acquire();  // Attend que le médiateur réveille ce fumeur
        cout << "Le fumeur avec le papier fabrique une cigarette.\n";
        this_thread::sleep_for(chrono::seconds(1));  // Simule la fabrication d'une cigarette
        cout << "Le fumeur avec le papier fume une cigarette.\n";
        agentSem.release();  // Réveille l'agent pour qu'il fournisse les prochains ingrédients
    }
}

// Fumeur avec allumettes
void smokerWithMatches() {
    while (true) {
        matchSmokerSem.acquire();  // Attend que le médiateur réveille ce fumeur
        cout << "Le fumeur avec les allumettes fabrique une cigarette.\n";
        this_thread::sleep_for(chrono::seconds(1));  // Simule la fabrication d'une cigarette
        cout << "Le fumeur avec les allumettes fume une cigarette.\n";
        agentSem.release();  // Réveille l'agent pour qu'il fournisse les prochains ingrédients
    }
}

// Médiateur
void mediator() {
    while (true) {
        lock_guard<mutex> lock(mtx);

        if (isTobacco && isPaper) {
            cout << "Réveille le fumeur avec allumettes\n";
            matchSmokerSem.release();
            isTobacco = isPaper = false;
        } else if (isTobacco && isMatch) {
            cout << "Réveille le fumeur avec papier\n";
            paperSmokerSem.release();
            isTobacco = isMatch = false;
        } else if (isPaper && isMatch) {
            cout << "Réveille le fumeur avec tabac\n";
            tobaccoSmokerSem.release();
            isPaper = isMatch = false;
        }
    }
}

// Agent
void agent() {
    while (true) {
        agentSem.acquire();
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
            cout << "Agent place les ingrédients : " << ingredients[ingredient1] << " et " << ingredients[ingredient2] << endl;
            if ((ingredient1 == 0 && ingredient2 == 1) || (ingredient1 == 1 && ingredient2 == 0)) {
                isTobacco = true;
                isPaper = true;
            } else if ((ingredient1 == 0 && ingredient2 == 2) || (ingredient1 == 2 && ingredient2 == 0)) {
                isTobacco = true;
                isMatch = true;
            } else if ((ingredient1 == 1 && ingredient2 == 2) || (ingredient1 == 2 && ingredient1 == 1)) {
                isPaper = true;
                isMatch = true;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}


int main() {
    // Démarrer les threads
    thread agentThread(agent);
    thread mediatorThread(mediator);
    thread smokerWithTobaccoThread(smokerWithTobacco);
    thread smokerWithPaperThread(smokerWithPaper);
    thread smokerWithMatchThread(smokerWithMatches);

    // Joindre les threads
    agentThread.join();
    mediatorThread.join();
    smokerWithTobaccoThread.join();
    smokerWithPaperThread.join();
    smokerWithMatchThread.join();

    return 0;
}
