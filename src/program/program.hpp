#pragma once
#include "../core/graphics/graphics.hpp"
#include <iostream>
#include <memory>

class Program
{
    std::unique_ptr<Graphics> graphics;
    double deltaTime;

public:
    Program();
    ~Program();
    void update(void);

private:
    void printFPS();
    double getDeltaTime();
};