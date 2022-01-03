#include "gameObject.hpp"
#include <iostream>

int32_t GameObject::globalId = 0;

GameObject::GameObject()
{
    id = GameObject::globalId++;
}

GameObject::~GameObject()
{
}

void GameObject::update(double deltaTime)
{
    std::cout << "updating game object " << id << "\n";
    for (auto &child : children)
    {
        child.update(deltaTime);
    }
}