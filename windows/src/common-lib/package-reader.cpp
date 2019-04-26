#include "stdafx.h"
#include "ZipLib/ZipArchive.h"
#include "holojs/private/error-handling.h"
#include "include/holojs/windows/windows-platform.h"
#include "package-reader.h"
#include <algorithm>
#include <codecvt>
#include <vector>

using namespace HoloJs::Platform::Win32;
using namespace std;


string PackageReader::to_string(const std::wstring& unicodeString)
{
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    return converter.to_bytes(unicodeString);
}

string PackageReader::to_lower_string(const wstring& unicodeString)
{
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    auto result = converter.to_bytes(unicodeString);
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);

    return result;
}

HRESULT PackageReader::initialize(HANDLE handle)
{
    RETURN_IF_FAILED(HoloJs::Platforms::WindowsPlatform::readfileBinaryFromHandle(handle, m_cachedPackage));
    
    m_packageBuffer.reset(new membuf(reinterpret_cast<char*>(m_cachedPackage.data()),
                                     reinterpret_cast<char*>(m_cachedPackage.data()) + m_cachedPackage.size()));
    
    m_packageStream = make_unique<std::istream>(m_packageBuffer.get());

    m_zipArchive = ZipArchive::Create(m_packageStream.get(), false);

    // TODO: this needs to be closed somewhere else
    CloseHandle(handle);

    return S_OK;
}

HRESULT PackageReader::initialize(const std::wstring packagePath)
{
	try
	{
		m_zipArchive = ZipFile::Open(to_string(packagePath));
	}
	catch (...) { return E_FAIL; }
    

    if (m_zipArchive) {
        return S_OK;
    } else {
        return E_FAIL;
    }
}

long PackageReader::readFileFromPackageUTF8(const std::wstring& filePath, std::wstring& text)
{
    std::lock_guard<std::mutex> guard(m_zipArchiveLock);

    auto entry = m_zipArchive->GetEntry(to_lower_string(filePath));

    RETURN_IF_NULL(entry.get());

    RETURN_IF_FALSE(entry->CanExtract());

    auto decompressor = entry->GetDecompressionStream();

    std::string ret;
    vector<char> readBuffer(1024);
    vector<wchar_t> unicodeReadBuffer(1024);
    while (decompressor->read(readBuffer.data(), readBuffer.size())) {
        auto convertedSize = MultiByteToWideChar(CP_UTF8,
                                                 0,
                                                 readBuffer.data(),
                                                 static_cast<int>(readBuffer.size()),
                                                 unicodeReadBuffer.data(),
                                                 static_cast<unsigned int>(unicodeReadBuffer.capacity()));
        RETURN_IF_TRUE(convertedSize == 0);
        text.append(unicodeReadBuffer.data(), convertedSize);
    }

    if (decompressor->gcount() > 0) {
        auto convertedSize = MultiByteToWideChar(CP_UTF8,
                                                 0,
                                                 readBuffer.data(),
                                                 static_cast<int>(decompressor->gcount()),
                                                 unicodeReadBuffer.data(),
                                                 static_cast<unsigned int>(unicodeReadBuffer.capacity()));
        if (convertedSize > 0) {
            text.append(unicodeReadBuffer.data(), convertedSize);
        }
    }

    entry->CloseDecompressionStream();

    return S_OK;
}

long PackageReader::readFileFromPackageUnicode(const std::wstring& filePath, std::wstring& text) { return E_FAIL; }

long PackageReader::readFileFromPackageBinary(const std::wstring& filePath, std::vector<unsigned char>& data)
{
    std::lock_guard<std::mutex> guard(m_zipArchiveLock);

    auto entry = m_zipArchive->GetEntry(to_lower_string(filePath));

    RETURN_IF_NULL(entry.get());

    RETURN_IF_FALSE(entry->CanExtract());

    auto decompressor = entry->GetDecompressionStream();

    std::string ret;
    vector<char> readBuffer(1024);
    while (decompressor->read(readBuffer.data(), readBuffer.size())) {
        data.insert(data.end(), readBuffer.begin(), readBuffer.begin() + readBuffer.size());
    }

    if (decompressor->gcount() > 0) {
        data.insert(data.end(), readBuffer.begin(), readBuffer.begin() + decompressor->gcount());
    }

    entry->CloseDecompressionStream();

    return S_OK;
}

HoloJs::IPackageReader* HoloJs::Platforms::WindowsPlatform::getPackageReaderFromPath(const std::wstring& packagePath)
{
    auto packageReader = new HoloJs::Platform::Win32::PackageReader();
    if (SUCCEEDED(packageReader->initialize(packagePath))) {
        return packageReader;
    } else {
        delete packageReader;
        return nullptr;
    }
}

HoloJs::IPackageReader* HoloJs::Platforms::WindowsPlatform::getPackageReaderFromHandle(void* platformHandle)
{
    auto packageReader = new HoloJs::Platform::Win32::PackageReader();
    if (SUCCEEDED(packageReader->initialize(platformHandle))) {
        return packageReader;
    } else {
        delete packageReader;
        return nullptr;
    }
}
