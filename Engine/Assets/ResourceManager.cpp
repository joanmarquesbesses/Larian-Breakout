#include "ResourceManager.h"

std::vector<std::function<void()>>& ResourceManager::GetCleanupQueue()
{
    static std::vector<std::function<void()>> queue;
    return queue;
}

std::vector<std::function<void()>>& ResourceManager::GetCleanUnusedQueue()
{
    static std::vector<std::function<void()>> queue;
    return queue;
}

void ResourceManager::Clear()
{
    for (auto& clearFunc : GetCleanupQueue())
    {
        clearFunc();
    }
    std::cout << "ResourceManager: All resources had been cleared.\n";
}

void ResourceManager::CleanUpUnused()
{
    for (auto& func : GetCleanUnusedQueue())
    {
        func();
    }
}