#pragma once

namespace HologramJS {
namespace Loaders {
class WebLoader {
   public:
    WebLoader();
    ~WebLoader();

    static concurrency::task<Platform::String ^> DownloadTextAsync(Windows::Foundation::Uri ^ uri);
};

}  // namespace Loaders
}  // namespace HologramJS
