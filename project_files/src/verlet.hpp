
#ifndef VERLET_HPP
#define VERLET_HPP
#pragma once

// std includes
#include <vector>

class Object;

struct VerletManager {
public:
    void CreateVerlets( int Amount );

    static VerletManager& Instance();

private:
    std::vector< Object* > verlet_list;
};

#endif
