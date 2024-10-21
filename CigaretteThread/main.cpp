#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>

using namespace std;

// Mutex et condition variable pour gérer la synchronisation
mutex mtx;
condition_variable cv;

bool hasTobacco = false;   // Indique si le tabac est disponible
bool hasPaper = false;     // Indique si le papier est disponible
bool hasMatches = false;   // Indique si les allumettes sont disponibles

void agent() {
    default_random_engine generator;
    uniform_int_distribution<int> distribution(0, 2); // Pour choisir deux ingrédients aléatoires

    while (true) {
        this_thread::sleep_for(chrono::seconds(2)); // Simuler le temps de choix des ingrédients

        // Choisir deux ingrédients aléatoires
        int first = distribution(generator);
        int second = distribution(generator);

        // S'assurer que les ingrédients choisis sont différents
        while (first == second) {
            second = distribution(generator);
        }

        // Mettre à disposition les ingrédients choisis
        {
            lock_guard<mutex> lock(mtx); // Protection de la section critique
            if (first == 0 && second == 1) { // Tabac et Papier
                hasTobacco = true;
                hasPaper = true;
                cout << "Agent: Tabac et Papier sont disponibles." << endl;
            } else if (first == 1 && second == 2) { // Papier et Allumettes
                hasPaper = true;
                hasMatches = true;
                cout << "Agent: Papier et Allumettes sont disponibles." << endl;
            } else if (first == 0 && second == 2) { // Tabac et Allumettes
                hasTobacco = true;
                hasMatches = true;
                cout << "Agent: Tabac et Allumettes sont disponibles." << endl;
            }
        }

        cv.notify_all(); // Notifier tous les fumeurs
    }
}

void smokerWithMatches() {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return hasTobacco && hasPaper; }); // Attendre que le tabac et le papier soient disponibles

        // Prendre les ingrédients
        hasTobacco = false; // Prendre le tabac
        hasPaper = false;   // Prendre le papier
        lock.unlock(); // Déverrouiller avant de fumer

        // Simuler le temps de fumage
        cout << "Fumeur avec Allumettes: Je fume une cigarette." << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
}

void smokerWithTobacco() {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return hasPaper && hasMatches; }); // Attendre que le papier et les allumettes soient disponibles

        // Prendre les ingrédients
        hasPaper = false;   // Prendre le papier
        hasMatches = false; // Prendre les allumettes
        lock.unlock(); // Déverrouiller avant de fumer

        // Simuler le temps de fumage
        cout << "Fumeur avec Tabac: Je fume une cigarette." << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
}

void smokerWithPaper() {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return hasTobacco && hasMatches; }); // Attendre que le tabac et les allumettes soient disponibles

        // Prendre les ingrédients
        hasTobacco = false; // Prendre le tabac
        hasMatches = false; // Prendre les allumettes
        lock.unlock(); // Déverrouiller avant de fumer

        // Simuler le temps de fumage
        cout << "Fumeur avec Papier: Je fume une cigarette." << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main() {
    // Création des threads pour l'agent et les fumeurs
    thread agentThread(agent);
    thread smoker1(smokerWithMatches);
    thread smoker2(smokerWithTobacco);
    thread smoker3(smokerWithPaper);

    // Attendre la fin des threads (jamais atteint dans ce cas)
    agentThread.join();
    smoker1.join();
    smoker2.join();
    smoker3.join();

    return 0;
}
