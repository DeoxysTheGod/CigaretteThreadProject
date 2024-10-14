#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <random>
#include <chrono>

// Sémaphores pour synchronisation
std::binary_semaphore tobaccoSem(0);
std::binary_semaphore paperSem(0);
std::binary_semaphore matchSem(0);
std::binary_semaphore agentSem(1);

// Mutex pour protéger l'accès aux variables partagées
std::mutex mtx;

// Compteurs pour suivre le nombre d'ingrédients sur la table
int numTobacco = 0, numPaper = 0, numMatch = 0;

void pusherA() {
    while (true) {
        tobaccoSem.acquire();
        std::lock_guard<std::mutex> lock(mtx);
        if (numPaper > 0) {
            numPaper--;
            matchSem.release();
        } else if (numMatch > 0) {
            numMatch--;
            paperSem.release();
        } else {
            numTobacco++;
        }
    }
}

void pusherB() {
    while (true) {
        paperSem.acquire();
        std::lock_guard<std::mutex> lock(mtx);
        if (numTobacco > 0) {
            numTobacco--;
            matchSem.release();
        } else if (numMatch > 0) {
            numMatch--;
            tobaccoSem.release();
        } else {
            numPaper++;
        }
    }
}

void pusherC() {
    while (true) {
        matchSem.acquire();
        std::lock_guard<std::mutex> lock(mtx);
        if (numTobacco > 0) {
            numTobacco--;
            paperSem.release();
        } else if (numPaper > 0) {
            numPaper--;
            tobaccoSem.release();
        } else {
            numMatch++;
        }
    }
}

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

        std::cout << "Agent place les ingrédients : " << ingredient1 << " et " << ingredient2 << std::endl;

        if ((ingredient1 == 0 && ingredient2 == 1) || (ingredient1 == 1 && ingredient2 == 0)) {
            matchSem.release();
        } else if ((ingredient1 == 0 && ingredient2 == 2) || (ingredient1 == 2 && ingredient2 == 0)) {
            paperSem.release();
        } else if ((ingredient1 == 1 && ingredient2 == 2) || (ingredient1 == 2 && ingredient1 == 1)) {
            tobaccoSem.release();
        }
    }
}

void smokerWithTobacco() {
    while (true) {
        tobaccoSem.acquire();
        std::cout << "Le fumeur avec le tabac fabrique une cigarette.\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        agentSem.release();
    }
}

void smokerWithPaper() {
    while (true) {
        paperSem.acquire();
        std::cout << "Le fumeur avec le papier fabrique une cigarette.\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        agentSem.release();
    }
}

void smokerWithMatch() {
    while (true) {
        matchSem.acquire();
        std::cout << "Le fumeur avec les allumettes fabrique une cigarette.\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        agentSem.release();
    }
}

int main() {
    std::thread agentThread(agent);
    std::thread pusherAThread(pusherA);
    std::thread pusherBThread(pusherB);
    std::thread pusherCThread(pusherC);
    std::thread smokerTobacco(smokerWithTobacco);
    std::thread smokerPaper(smokerWithPaper);
    std::thread smokerMatch(smokerWithMatch);

    agentThread.join();
    pusherAThread.join();
    pusherBThread.join();
    pusherCThread.join();
    smokerTobacco.join();
    smokerPaper.join();
    smokerMatch.join();

    return 0;
}
