#include "Coroutine.h"

void CoroutineManager::StartCoroutine(float duration, std::function<void()> callback)
{
    Coroutine c;
    c.duration = duration;
    c.timer = 0.0f;
    c.finished = false;
    c.callback = callback;

    coroutines.push_back(c);
}

void CoroutineManager::Update(float deltaTime)
{
    // Actualiza todas las corutinas
    for (auto& c : coroutines)
    {
        if (!c.finished)
        {
            c.timer += deltaTime;
            if (c.timer >= c.duration)
            {
                if (c.callback)
                    c.callback();
                c.finished = true;
            }
        }
    }

    // Elimina todas las corutinas terminadas
    coroutines.erase(
        std::remove_if(coroutines.begin(), coroutines.end(),
            [](const Coroutine& c) { return c.finished; }),
        coroutines.end()
    );
}
