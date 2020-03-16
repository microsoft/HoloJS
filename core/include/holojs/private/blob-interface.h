#pragma once

#include <memory>
#include <vector>
#include <string>

namespace HoloJs {

class IBlob {
   public:
    virtual std::shared_ptr<std::vector<unsigned char>> data() = 0;
    virtual std::wstring mimeType() = 0;
};

}  // namespace HoloJs