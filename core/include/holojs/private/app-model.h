#pragma once

#include "package-reader.h"
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace HoloJs {
namespace AppModel {

class Script {
   public:
    Script(const std::wstring& path, std::wstring&& code) : m_path(path), m_code(std::move(code)) {}
    Script(const Script&) = delete;

    const std::wstring getPath() const { return m_path; }
    const std::wstring getCode() const { return m_code; }
	const std::wstring getName() const { return m_name; }
	void setName(const std::wstring& name) { m_name = name; }

   private:
    std::wstring m_path;
    std::wstring m_code;
	std::wstring m_name;
};

enum class AppSource { Web, FileSystem, Package };

struct AppConfiguration {
    AppSource source;
    std::wstring baseUri;
    std::shared_ptr<HoloJs::IPackageReader> packageReader;
};

class HoloJsApp {
   public:
    HoloJsApp(AppConfiguration& appConfiguration) : m_appConfiguration(appConfiguration)
    {
        m_loadedScripts = std::make_shared<std::list<Script>>();
    }

    HoloJsApp()
    {
        m_appConfiguration.baseUri = L"";
        m_appConfiguration.source = AppSource::FileSystem;
        m_loadedScripts = std::make_shared<std::list<Script>>();
    }

    ~HoloJsApp() {}

    std::shared_ptr<std::list<Script>> loadedScriptsList() { return m_loadedScripts; }
    const AppConfiguration& getAppConfiguration() { return m_appConfiguration; }

    std::wstring getName() { return m_name; }
    void setName(const std::wstring& name) { m_name = name; }

    std::wstring getIconUri() { return m_iconUri; }
    void setIconUri(std::wstring& iconUri) { m_iconUri = iconUri; }

   private:
    std::shared_ptr<std::list<Script>> m_loadedScripts;
    AppConfiguration m_appConfiguration;

    std::wstring m_name;
    std::wstring m_iconUri;
};

}  // namespace AppModel
}  // namespace HoloJs