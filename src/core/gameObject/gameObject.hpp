#include <vector>
#include <iostream>

class GameObject
{
    static int32_t globalId;

public:
    int32_t id;
    std::vector<GameObject> children;
    GameObject();
    ~GameObject() = default;
    void update(double deltaTime);
};