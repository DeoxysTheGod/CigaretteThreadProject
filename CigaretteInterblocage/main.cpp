#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <random>
#include <chrono>
#include <map>

// Sémaphores pour les ingrédients et la synchronisation
std::binary_semaphore tobaccoSem(0);
std::binary_semaphore paperSem(0);
std::binary_semaphore matchSem(0);
std::binary_semaphore agentSem(1);

// Mutex pour la protection des variables partagées
std::mutex mtx;
bool isTobacco = false, isPaper = false, isMatch = false;


// afficher les ingrédients
std::map<unsigned, std::string> ingredients = {
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
        paperSem.acquire();
        matchSem.acquire();
        std::cout << "Le fumeur avec le tabac fabrique une cigarette.\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Simule la fabrication d'une cigarette
        agentSem.release();  // Réveille l'agent pour qu'il fournisse les prochains ingrédients
        std::cout << "Le fumeur avec le tabac fume une cigarette.\n";
    }
}

// Fumeur avec papier (a besoin de tabac et d'allumettes)
void smokerWithPaper() {
    while (true) {
        tobaccoSem.acquire();
        matchSem.acquire();
        std::cout << "Le fumeur avec le papier fabrique une cigarette.\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Simule la fabrication d'une cigarette
        agentSem.release();  // Réveille l'agent pour qu'il fournisse les prochains ingrédients
        std::cout << "Le fumeur avec le papier fume une cigarette.\n";
    }
}

// Fumeur avec allumettes (a besoin de tabac et de papier)
void smokerWithMatches() {
    while (true) {
        tobaccoSem.acquire();
        paperSem.acquire();
        std::cout << "Le fumeur avec les allumettes fabrique une cigarette.\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Simule la fabrication d'une cigarette
        agentSem.release();  // Réveille l'agent pour qu'il fournisse les prochains ingrédients
        std::cout << "Le fumeur avec les allumettes fume une cigarette.\n";
    }
}
// Agent qui place les ingrédients
void agent() {
    while (true) {
        agentSem.acquire();
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 2);
        int ingredient1 = dist(gen);
        int ingredient2 = dist(gen);
        while (ingredient1 == ingredient2) {
            ingredient2 = dist(gen);
        }

        std::cout << "Agent place les ingrédients : " << ingredients[ingredient1] << " et " << ingredients[ingredient2] << std::endl;

        if ((ingredient1 == 0 && ingredient2 == 1) || (ingredient1 == 1 && ingredient2 == 0)) {
            // matchSem.release();
            tobaccoSem.release();
            paperSem.release();
        } else if ((ingredient1 == 0 && ingredient2 == 2) || (ingredient1 == 2 && ingredient2 == 0)) {
            // paperSem.release();
            tobaccoSem.release();
            matchSem.release();
        } else if ((ingredient1 == 1 && ingredient2 == 2) || (ingredient1 == 2 && ingredient1 == 1)) {
            // tobaccoSem.release();
            paperSem.release();
            std::thread smokerWithTobaccoThread(smokerWithTobacco);
            std::thread smokerWithPaperThread(smokerWithPaper);
            std::thread smokerWithMatchThread(smokerWithMatches);

            matchSem.release();
        }
    }
}

int main() {
    // Démarrer les threads
    std::thread agentThread(agent);
    std::thread smokerWithTobaccoThread(smokerWithTobacco);
    std::thread smokerWithPaperThread(smokerWithPaper);
    std::thread smokerWithMatchThread(smokerWithMatches);

    // Joindre les threads
    agentThread.join();
    smokerWithTobaccoThread.join();
    smokerWithPaperThread.join();
    smokerWithMatchThread.join();

    return 0;
}
