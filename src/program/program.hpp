#pragma once
#include "../core/graphics/graphics.hpp"
#include <iostream>
#include <memory>

class Program
{
    Graphics *graphics;

public:
    Program();
    ~Program();
    void update(void);
};