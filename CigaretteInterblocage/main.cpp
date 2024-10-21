#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <random>
#include <chrono>
#include <map>

using namespace std;

// Sémaphores pour les ingrédients et la synchronisation
binary_semaphore tobaccoSem(0);
binary_semaphore paperSem(0);
binary_semaphore matchSem(0);
binary_semaphore agentSem(1);

// Mutex pour la protection des variables partagées
mutex mtx;
bool isTobacco = false, isPaper = false, isMatch = false;


// afficher les ingrédients
map<unsigned, string> ingredients = {
    {0, "Tobacco"},
    {1, "Paper"},
    {2, "Match"}
};

//---------------------------//
//          Fumeurs          //
//---------------------------//

// Fumeur avec tabac (a besoin de papier et d'allumettes)
void smokerWithTobacco() {
    while (true) {
        {
            lock_guard<mutex> lock(mtx);  // Assure une acquisition atomique des sémaphores
            paperSem.acquire();
            matchSem.acquire();
        }
        cout << "Le fumeur avec le tabac fabrique une cigarette.\n";
        this_thread::sleep_for(chrono::seconds(1));
        agentSem.release();
        cout << "Le fumeur avec le tabac fume une cigarette.\n";
    }
}

void smokerWithPaper() {
    while (true) {
        {
            lock_guard<mutex> lock(mtx);  // Assure une acquisition atomique des sémaphores
            tobaccoSem.acquire();
            matchSem.acquire();
        }
        cout << "Le fumeur avec le papier fabrique une cigarette.\n";
        this_thread::sleep_for(chrono::seconds(1));
        agentSem.release();
        cout << "Le fumeur avec le papier fume une cigarette.\n";
    }
}

void smokerWithMatches() {
    while (true) {
        {
            lock_guard<mutex> lock(mtx);  // Assure une acquisition atomique des sémaphores
            tobaccoSem.acquire();
            paperSem.acquire();
        }
        cout << "Le fumeur avec les allumettes fabrique une cigarette.\n";
        this_thread::sleep_for(chrono::seconds(1));
        agentSem.release();
        cout << "Le fumeur avec les allumettes fume une cigarette.\n";
    }
}
// Agent qui place les ingrédients
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

        cout << "Agent place les ingrédients : " << ingredients[ingredient1] << " et " << ingredients[ingredient2] << endl;

        if ((ingredient1 == 0 && ingredient2 == 1) || (ingredient1 == 1 && ingredient2 == 0)) {
            tobaccoSem.release();
            paperSem.release();
        } else if ((ingredient1 == 0 && ingredient2 == 2) || (ingredient1 == 2 && ingredient2 == 0)) {
            tobaccoSem.release();
            matchSem.release();
        } else if ((ingredient1 == 1 && ingredient2 == 2) || (ingredient1 == 2 && ingredient1 == 1)) {
            paperSem.release();
            matchSem.release();
        }
    }
}

int main() {
    // Démarrer les threads
    thread agentThread(agent);
    thread smokerWithTobaccoThread(smokerWithTobacco);
    thread smokerWithPaperThread(smokerWithPaper);
    thread smokerWithMatchThread(smokerWithMatches);

    // Joindre les threads
    agentThread.join();
    smokerWithTobaccoThread.join();
    smokerWithPaperThread.join();
    smokerWithMatchThread.join();

    return 0;
}
