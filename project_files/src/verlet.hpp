
#ifndef VERLET_HPP
#define VERLET_HPP
#pragma once

struct VerletManager {
public:
    void CheckCollisions();

    static VerletManager& Instance();
private:

};

#endif
