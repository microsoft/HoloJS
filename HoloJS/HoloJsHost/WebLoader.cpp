#include "pch.h"
#include "WebLoader.h"

using namespace concurrency;
using namespace std;
using namespace Windows::Web;
using namespace Windows::Foundation;
using namespace Windows::Web::Http;
using namespace HologramJS::Loaders;

WebLoader::WebLoader() {}

WebLoader::~WebLoader() {}

task<Platform::String ^> WebLoader::DownloadTextAsync(Uri ^ uri)
{
    Http::HttpClient ^ httpClient = ref new HttpClient();
    auto responseMessage = await httpClient->GetAsync(uri);
    if (responseMessage->IsSuccessStatusCode) {
        return await responseMessage->Content->ReadAsStringAsync();
    }

    return L"";
}
