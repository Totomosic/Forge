#include "ForgePch.h"
#include "FileUtils.h"

#include <fstream>

namespace Forge
{

    bool FileUtils::Exists(const std::string& filepath)
    {
        std::ifstream file(filepath);
        return file.good();
    }

    std::string FileUtils::ReadTextFile(const std::string& filepath)
    {
        FORGE_ASSERT(Exists(filepath), "File {} does not exist", filepath);
        std::ifstream file(filepath);
        return std::string(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
        );
    }

}
