#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>

std::mutex mtx;
std::condition_variable cv;
bool hasTobacco = false;
bool hasPaper = false;
bool hasMatches = false;

void agent() {
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, 2);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // Choisir deux ingrédients aléatoires
        int first = distribution(generator);
        int second = distribution(generator);
        while (first == second) {
            second = distribution(generator);
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            if (first == 0 && second == 1) {
                hasTobacco = true;
                hasPaper = true;
                std::cout << "Agent: Tabac et Papier sont disponibles." << std::endl;
            } else if (first == 1 && second == 2) {
                hasPaper = true;
                hasMatches = true;
                std::cout << "Agent: Papier et Allumettes sont disponibles." << std::endl;
            } else if (first == 0 && second == 2) {
                hasTobacco = true;
                hasMatches = true;
                std::cout << "Agent: Tabac et Allumettes sont disponibles." << std::endl;
            }
        }

        cv.notify_all();
    }
}

void smokerWithMatches() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return hasTobacco && hasPaper; });

        // Simuler la prise de ressources
        hasTobacco = false; // Prendre le tabac
        hasPaper = false;   // Prendre le papier
        lock.unlock(); // Déverrouiller avant de fumer

        std::cout << "Fumeur avec Allumettes: Je fume une cigarette." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void smokerWithTobacco() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return hasPaper && hasMatches; });

        // Simuler la prise de ressources
        hasPaper = false;   // Prendre le papier
        hasMatches = false; // Prendre les allumettes
        lock.unlock(); // Déverrouiller avant de fumer

        std::cout << "Fumeur avec Tabac: Je fume une cigarette." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void smokerWithPaper() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return hasTobacco && hasMatches; });

        // Simuler la prise de ressources
        hasTobacco = false; // Prendre le tabac
        hasMatches = false; // Prendre les allumettes
        lock.unlock(); // Déverrouiller avant de fumer

        std::cout << "Fumeur avec Papier: Je fume une cigarette." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    std::thread agentThread(agent);
    std::thread smoker1(smokerWithMatches);
    std::thread smoker2(smokerWithTobacco);
    std::thread smoker3(smokerWithPaper);

    agentThread.join();
    smoker1.join();
    smoker2.join();
    smoker3.join();

    return 0;
}
