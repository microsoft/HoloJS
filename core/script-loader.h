#pragma once

#include <list>
#include <memory>
#include <string>
#include <vector>
#include "include/holojs/private/app-model.h"

namespace HoloJs {
namespace AppModel {

class ScriptsLoader {
   public:
    ScriptsLoader() {}

    long loadApp(const std::wstring& appUri, std::shared_ptr<HoloJsApp>& scriptApp);
    long loadApp(void* platformHandle, std::shared_ptr<HoloJsApp>& scriptApp);
    long createEmptyApp(std::shared_ptr<HoloJsApp>& scriptApp);
    long loadScriptInline(const std::wstring& script, std::shared_ptr<HoloJsApp>& scriptApp);
    void setSupportScripts(std::shared_ptr<std::list<HoloJs::AppModel::Script>> supportScripts) { m_supportScripts = supportScripts; }

    long createAppFromScripts(const std::wstring& appName,
                              std::shared_ptr<std::list<HoloJs::AppModel::Script>> scripts,
                              std::shared_ptr<HoloJsApp>& scriptApp);

    static bool isAbsoluteWebUri(std::wstring appUri);

   private:

    std::shared_ptr<std::list<HoloJs::AppModel::Script>> m_supportScripts;

    std::wstring getFileSystemBasePathForAppPath(const std::wstring& appPath);
    std::wstring getBaseUriForAppUri(const std::wstring& appUri);

    std::list<std::wstring> splitPath(const std::wstring& path);
};
}  // namespace AppModel
}  // namespace HoloJs