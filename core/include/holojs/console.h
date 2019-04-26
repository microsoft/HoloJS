#pragma once

namespace HoloJs {
enum class ConsoleCallbackMode { RunDefaultBehavior, OverrideDefaultBehavior };

class IConsoleConfiguration {
   public:
    virtual ~IConsoleConfiguration() {}

    virtual void onConsoleLog(const wchar_t* message) = 0;
    virtual void onConsoleDebug(const wchar_t* message) = 0;

    ConsoleCallbackMode callbackMode;
};
}  // namespace HoloJs