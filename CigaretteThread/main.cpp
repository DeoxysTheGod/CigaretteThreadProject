#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <chrono>

std::mutex mtx;
std::condition_variable tobacco_cv, paper_cv, match_cv, agent_cv;
bool is_tobacco = false, is_paper = false, is_match = false;
bool agent_ready = true;

// Fonction de l'agent qui place deux ingrédients sur la table
void agent() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulation d'une action
        std::unique_lock<std::mutex> lock(mtx);
        agent_cv.wait(lock, [] { return agent_ready; });

        // Choisir aléatoirement deux ingrédients
        int choice = rand() % 3;
        if (choice == 0) {
            is_tobacco = true;
            is_paper = true;
            std::cout << "Agent met tabac et papier sur la table.\n";
            match_cv.notify_one();  // Le fumeur avec les allumettes est réveillé
        } else if (choice == 1) {
            is_tobacco = true;
            is_match = true;
            std::cout << "Agent met tabac et allumettes sur la table.\n";
            paper_cv.notify_one();  // Le fumeur avec le papier est réveillé
        } else {
            is_paper = true;
            is_match = true;
            std::cout << "Agent met papier et allumettes sur la table.\n";
            tobacco_cv.notify_one();  // Le fumeur avec le tabac est réveillé
        }
        agent_ready = false;  // L'agent attend que le fumeur ait fini
    }
}

// Fumeur avec le tabac
void smoker_with_tobacco() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        tobacco_cv.wait(lock, [] { return is_paper && is_match; });
        std::cout << "Fumeur avec tabac fabrique et fume une cigarette.\n";
        is_paper = is_match = false;  // Consomme les ingrédients
        agent_ready = true;
        agent_cv.notify_one();  // Réveille l'agent pour le prochain tour
    }
}

// Fumeur avec le papier
void smoker_with_paper() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        paper_cv.wait(lock, [] { return is_tobacco && is_match; });
        std::cout << "Fumeur avec papier fabrique et fume une cigarette.\n";
        is_tobacco = is_match = false;
        agent_ready = true;
        agent_cv.notify_one();
    }
}

// Fumeur avec les allumettes
void smoker_with_match() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        match_cv.wait(lock, [] { return is_tobacco && is_paper; });
        std::cout << "Fumeur avec allumettes fabrique et fume une cigarette.\n";
        is_tobacco = is_paper = false;
        agent_ready = true;
        agent_cv.notify_one();
    }
}

int main() {
    std::thread agent_thread(agent);
    std::thread smoker_tobacco(smoker_with_tobacco);
    std::thread smoker_paper(smoker_with_paper);
    std::thread smoker_match(smoker_with_match);

    agent_thread.join();
    smoker_tobacco.join();
    smoker_paper.join();
    smoker_match.join();

    return 0;
}
