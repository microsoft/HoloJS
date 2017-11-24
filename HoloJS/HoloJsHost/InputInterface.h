#pragma once

namespace HologramJS {
namespace Input {
// the mapping of events to values is fixed on the script side;
// any changes here must be ported to window.js as well
enum class NativeToScriptInputType : int { Resize = 0, Mouse, Keyboard, SpatialInput, SpatialMapping, VSync, Voice };

enum class KeyboardInputEventType : int { KeyDown = 0, KeyUp };

enum class MouseInputEventType : int { MouseUp = 0, MouseDown, MouseMove, MouseWheel };

enum class SpatialInputEventType : int { Pressed = 0, Released, Lost, Detected, Update };

enum class VoiceInputEventType : int { Command = 0 };

}  // namespace Input
}  // namespace HologramJS
