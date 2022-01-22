#include "../core/graphics/graphics.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include "shape/shape.hpp"

class Program
{
    std::unique_ptr<Graphics> graphics;
    double deltaTime = 0;

public:
    Program();
    ~Program();
    void update(void);

private:
    void printFPS();
    double getDeltaTime();
};