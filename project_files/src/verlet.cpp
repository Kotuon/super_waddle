
// Local includes
#include "verlet.hpp"

void VerletManager::CheckCollisions() {
    
}

VerletManager& VerletManager::Instance() {
    static VerletManager verletManagerInstance;
    return verletManagerInstance;
}
