#pragma once
#include <vector>
#include <functional>
#include <algorithm>

struct Coroutine
{
    float duration = 0.0f;
    float timer = 0.0f;
    bool finished = false;
    std::function<void()> callback;
};

class CoroutineManager
{
public:
    void StartCoroutine(float duration, std::function<void()> callback);
    void Update(float deltaTime);

private:
    std::vector<Coroutine> coroutines;
};
