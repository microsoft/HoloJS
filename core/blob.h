#pragma once

#include "include/holojs/private/blob-interface.h"
#include "include/holojs/private/chakra.h"
#include "include/holojs/private/object-lifetime.h"
#include <memory>
#include <vector>

namespace HoloJs {
namespace Interfaces {

class BlobProjection {
   public:
    BlobProjection(std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager)
        : m_resourceManager(resourceManager){}

	long initialize();

   private:
    JS_PROJECTION_DEFINE(BlobProjection, create)
    std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> m_resourceManager;
};

class Blob : public HoloJs::ResourceManagement::IRelease, public HoloJs::IBlob {
   public:
    virtual ~Blob() {}
    static Blob* fromScriptData(JsValueRef blobParts,
                                JsValueRef options,
                                std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager);

    virtual std::shared_ptr<std::vector<unsigned char>> data() { return m_data; }
    virtual std::wstring mimeType() { return m_mimeType; }

    void Release() {}

    Blob() {}

   private:

    std::shared_ptr<std::vector<unsigned char>> m_data;
    std::wstring m_mimeType;
};

}  // namespace Interfaces
}  // namespace HoloJs
