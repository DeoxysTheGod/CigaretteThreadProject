#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>

// Mutex et condition variable pour gérer la synchronisation
std::mutex mtx;
std::condition_variable cv;

bool hasTobacco = false;   // Indique si le tabac est disponible
bool hasPaper = false;     // Indique si le papier est disponible
bool hasMatches = false;   // Indique si les allumettes sont disponibles

void agent() {
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, 2); // Pour choisir deux ingrédients aléatoires

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2)); // Simuler le temps de choix des ingrédients

        // Choisir deux ingrédients aléatoires
        int first = distribution(generator);
        int second = distribution(generator);

        // S'assurer que les ingrédients choisis sont différents
        while (first == second) {
            second = distribution(generator);
        }

        // Mettre à disposition les ingrédients choisis
        {
            std::lock_guard<std::mutex> lock(mtx); // Protection de la section critique
            if (first == 0 && second == 1) { // Tabac et Papier
                hasTobacco = true;
                hasPaper = true;
                std::cout << "Agent: Tabac et Papier sont disponibles." << std::endl;
            } else if (first == 1 && second == 2) { // Papier et Allumettes
                hasPaper = true;
                hasMatches = true;
                std::cout << "Agent: Papier et Allumettes sont disponibles." << std::endl;
            } else if (first == 0 && second == 2) { // Tabac et Allumettes
                hasTobacco = true;
                hasMatches = true;
                std::cout << "Agent: Tabac et Allumettes sont disponibles." << std::endl;
            }
        }

        cv.notify_all(); // Notifier tous les fumeurs
    }
}

void smokerWithMatches() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return hasTobacco && hasPaper; }); // Attendre que le tabac et le papier soient disponibles

        // Prendre les ingrédients
        hasTobacco = false; // Prendre le tabac
        hasPaper = false;   // Prendre le papier
        lock.unlock(); // Déverrouiller avant de fumer

        // Simuler le temps de fumage
        std::cout << "Fumeur avec Allumettes: Je fume une cigarette." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void smokerWithTobacco() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return hasPaper && hasMatches; }); // Attendre que le papier et les allumettes soient disponibles

        // Prendre les ingrédients
        hasPaper = false;   // Prendre le papier
        hasMatches = false; // Prendre les allumettes
        lock.unlock(); // Déverrouiller avant de fumer

        // Simuler le temps de fumage
        std::cout << "Fumeur avec Tabac: Je fume une cigarette." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void smokerWithPaper() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return hasTobacco && hasMatches; }); // Attendre que le tabac et les allumettes soient disponibles

        // Prendre les ingrédients
        hasTobacco = false; // Prendre le tabac
        hasMatches = false; // Prendre les allumettes
        lock.unlock(); // Déverrouiller avant de fumer

        // Simuler le temps de fumage
        std::cout << "Fumeur avec Papier: Je fume une cigarette." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    // Création des threads pour l'agent et les fumeurs
    std::thread agentThread(agent);
    std::thread smoker1(smokerWithMatches);
    std::thread smoker2(smokerWithTobacco);
    std::thread smoker3(smokerWithPaper);

    // Attendre la fin des threads (jamais atteint dans ce cas)
    agentThread.join();
    smoker1.join();
    smoker2.join();
    smoker3.join();

    return 0;
}
