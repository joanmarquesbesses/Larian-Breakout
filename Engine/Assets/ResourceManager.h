#pragma once

#include <unordered_map>
#include <string>
#include <filesystem>
#include <memory>
#include <functional>
#include <vector>
#include <iostream>
#include <algorithm> 

class ResourceManager
{
public:
    template<typename T>
    static std::shared_ptr<T> Get(const std::filesystem::path& path)
    {
        auto& cache = GetCache<T>();

        // Normaliza path so "textures\bat.png" is the same as "textures/bat.png"
        std::string pathString = path.string();
        std::replace(pathString.begin(), pathString.end(), '\\', '/');

        if (cache.find(pathString) != cache.end())
        {
            return cache[pathString];
        }

        if (!std::filesystem::exists(path))
        {
            std::cout << "ResourceManager Warning: Asset not found at " << pathString << "\n";
            return nullptr;
        }

        std::shared_ptr<T> newAsset = std::make_shared<T>(pathString);

        cache[pathString] = newAsset;
        return newAsset;
    }

    static void Clear();
    static void CleanUpUnused();

private:
    static std::vector<std::function<void()>>& GetCleanupQueue();
    static std::vector<std::function<void()>>& GetCleanUnusedQueue();

    template<typename T>
    static std::unordered_map<std::string, std::shared_ptr<T>>& GetCache()
    {
        static std::unordered_map<std::string, std::shared_ptr<T>> cache;

        static bool isRegistered = false;
        if (!isRegistered)
        {
            // Clean all T resources
            GetCleanupQueue().push_back([]() {
                GetCache<T>().clear();
                });

            // Free resources that have no use
            GetCleanUnusedQueue().push_back([]() {
                auto& currentCache = GetCache<T>();
                for (auto it = currentCache.begin(); it != currentCache.end(); )
                {
                    // If is just kept by the resourcemanager (use_count == 1) will be erased
                    if (it->second.use_count() == 1) {
                        it = currentCache.erase(it);
                    }
                    else {
                        ++it;
                    }
                }
                });
            isRegistered = true;
        }

        return cache;
    }
};