#pragma once

#include <vector>
#include <string>

namespace HoloJs {

class IPackageReader {
   public:
    virtual long readFileFromPackageUTF8(const std::wstring& filePath, std::wstring& text) = 0;
    virtual long readFileFromPackageUnicode(const std::wstring& filePath, std::wstring& text) = 0;
    virtual long readFileFromPackageBinary(const std::wstring& filePath, std::vector<unsigned char>& data) = 0;
};

}  // namespace HoloJs