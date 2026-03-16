#pragma once
#include <string>
#include <memory>

class AudioClip {
public:
    AudioClip(const std::string& path) : m_Path(path) {}

    const std::string& GetPath() const { return m_Path; }

private:
    std::string m_Path;
};