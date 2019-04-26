#pragma once

#include "ZipLib/ZipFile.h"
#include "holojs/private/package-reader.h"
#include <istream>
#include <streambuf>
#include <memory>
#include <vector>
#include <mutex>

namespace HoloJs {
namespace Platform {
namespace Win32 {

struct membuf : std::streambuf {
    membuf(char* begin, char* end) : begin(begin), end(end) { this->setg(begin, begin, end); }

    virtual pos_type seekoff(std::streambuf::off_type off,
                             std::ios_base::seekdir dir,
                             std::ios_base::openmode which = std::ios_base::in) override
    {
        if (dir == std::ios_base::cur)
            gbump(off);
        else if (dir == std::ios_base::end)
            setg(begin, end + off, end);
        else if (dir == std::ios_base::beg)
            setg(begin, begin + off, end);

        return gptr() - eback();
    }

    virtual pos_type seekpos(std::streampos pos, std::ios_base::openmode mode) override
    {
        return seekoff(pos - pos_type(off_type(0)), std::ios_base::beg, mode);
    }

    char *begin, *end;
};

class PackageReader : public HoloJs::IPackageReader {
   public:
    PackageReader() {}
    ~PackageReader() {}

    HRESULT initialize(const std::wstring packagePath);
    HRESULT initialize(HANDLE handle);

    virtual long readFileFromPackageUTF8(const std::wstring& filePath, std::wstring& text);
    virtual long readFileFromPackageUnicode(const std::wstring& filePath, std::wstring& text);
    virtual long readFileFromPackageBinary(const std::wstring& filePath, std::vector<unsigned char>& data);

   private:
    std::shared_ptr<ZipArchive> m_zipArchive;

    std::vector<unsigned char> m_cachedPackage;
    std::unique_ptr<membuf> m_packageBuffer;
    std::unique_ptr<std::istream> m_packageStream;

    std::string to_string(const std::wstring& unicodeString);
    std::string to_lower_string(const std::wstring& unicodeString);

    std::mutex m_zipArchiveLock;
};

}  // namespace Win32
}  // namespace Platform
}  // namespace HoloJs