#pragma once

#include "include/holojs/private/chakra.h"
#include "resource-management/resource-manager.h"

namespace HoloJs {
class CanvasProjections {
   public:
    CanvasProjections(std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager)
        : m_resourceManager(resourceManager)
    {
    }
    ~CanvasProjections() {}

    long initialize();

   private:
    std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> m_resourceManager;

    JsValueRef m_createContext2D;
    static JsValueRef CHAKRA_CALLBACK staticCreateContext2D(JsValueRef callee,
                                                            bool isConstructCall,
                                                            JsValueRef* arguments,
                                                            unsigned short argumentCount,
                                                            PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->createContext2D(arguments, argumentCount);
    }
    JsValueRef createContext2D(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_drawImage;
    static JsValueRef CHAKRA_CALLBACK staticDrawImage(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->drawImage(arguments, argumentCount);
    }
    JsValueRef drawImage(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_clearRect;
    static JsValueRef CHAKRA_CALLBACK staticclearRect(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->clearRect(arguments, argumentCount);
    }
    JsValueRef clearRect(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_fillRect;
    static JsValueRef CHAKRA_CALLBACK staticfillRect(JsValueRef callee,
                                                     bool isConstructCall,
                                                     JsValueRef* arguments,
                                                     unsigned short argumentCount,
                                                     PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->fillRect(arguments, argumentCount);
    }
    JsValueRef fillRect(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_fillRectGradient;
    static JsValueRef CHAKRA_CALLBACK staticfillRectGradient(JsValueRef callee,
                                                             bool isConstructCall,
                                                             JsValueRef* arguments,
                                                             unsigned short argumentCount,
                                                             PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->fillRectGradient(arguments, argumentCount);
    }
    JsValueRef fillRectGradient(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_fillText;
    static JsValueRef CHAKRA_CALLBACK staticfillText(JsValueRef callee,
                                                     bool isConstructCall,
                                                     JsValueRef* arguments,
                                                     unsigned short argumentCount,
                                                     PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->fillText(arguments, argumentCount);
    }
    JsValueRef fillText(JsValueRef* arguments, unsigned short argumentCount);
    JsValueRef m_measureText;
    static JsValueRef CHAKRA_CALLBACK staticmeasureText(JsValueRef callee,
                                                        bool isConstructCall,
                                                        JsValueRef* arguments,
                                                        unsigned short argumentCount,
                                                        PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->measureText(arguments, argumentCount);
    }
    JsValueRef measureText(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getImageData;
    static JsValueRef CHAKRA_CALLBACK staticgetImageData(JsValueRef callee,
                                                         bool isConstructCall,
                                                         JsValueRef* arguments,
                                                         unsigned short argumentCount,
                                                         PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->getImageData(arguments, argumentCount);
    }
    JsValueRef getImageData(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getWidth;
    static JsValueRef CHAKRA_CALLBACK staticgetWidth(JsValueRef callee,
                                                     bool isConstructCall,
                                                     JsValueRef* arguments,
                                                     unsigned short argumentCount,
                                                     PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->getWidth(arguments, argumentCount);
    }
    JsValueRef getWidth(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_setWidth;
    static JsValueRef CHAKRA_CALLBACK staticsetWidth(JsValueRef callee,
                                                     bool isConstructCall,
                                                     JsValueRef* arguments,
                                                     unsigned short argumentCount,
                                                     PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->setWidth(arguments, argumentCount);
    }
    JsValueRef setWidth(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getHeight;
    static JsValueRef CHAKRA_CALLBACK staticgetHeight(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->getHeight(arguments, argumentCount);
    }
    JsValueRef getHeight(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_setHeight;
    static JsValueRef CHAKRA_CALLBACK staticsetHeight(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->setHeight(arguments, argumentCount);
    }
    JsValueRef setHeight(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_toDataUrl;
    static JsValueRef CHAKRA_CALLBACK statictoDataUrl(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->toDataUrl(arguments, argumentCount);
    }
    JsValueRef toDataUrl(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_beginPath;
    static JsValueRef CHAKRA_CALLBACK staticbeginPath(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->beginPath(arguments, argumentCount);
    }
    JsValueRef beginPath(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_closePath;
    static JsValueRef CHAKRA_CALLBACK staticclosePath(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->closePath(arguments, argumentCount);
    }
    JsValueRef closePath(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_moveTo;
    static JsValueRef CHAKRA_CALLBACK staticmoveTo(JsValueRef callee,
                                                   bool isConstructCall,
                                                   JsValueRef* arguments,
                                                   unsigned short argumentCount,
                                                   PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->moveTo(arguments, argumentCount);
    }
    JsValueRef moveTo(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_lineTo;
    static JsValueRef CHAKRA_CALLBACK staticlineTo(JsValueRef callee,
                                                   bool isConstructCall,
                                                   JsValueRef* arguments,
                                                   unsigned short argumentCount,
                                                   PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->lineTo(arguments, argumentCount);
    }
    JsValueRef lineTo(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_bezierCurveTo;
    static JsValueRef CHAKRA_CALLBACK staticbezierCurveTo(JsValueRef callee,
                                                          bool isConstructCall,
                                                          JsValueRef* arguments,
                                                          unsigned short argumentCount,
                                                          PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->bezierCurveTo(arguments, argumentCount);
    }
    JsValueRef bezierCurveTo(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_quadraticCurveTo;
    static JsValueRef CHAKRA_CALLBACK staticquadraticCurveTo(JsValueRef callee,
                                                             bool isConstructCall,
                                                             JsValueRef* arguments,
                                                             unsigned short argumentCount,
                                                             PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->quadraticCurveTo(arguments, argumentCount);
    }
    JsValueRef quadraticCurveTo(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_arc;
    static JsValueRef CHAKRA_CALLBACK staticarc(JsValueRef callee,
                                                bool isConstructCall,
                                                JsValueRef* arguments,
                                                unsigned short argumentCount,
                                                PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->arc(arguments, argumentCount);
    }
    JsValueRef arc(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_fill;
    static JsValueRef CHAKRA_CALLBACK staticfill(JsValueRef callee,
                                                 bool isConstructCall,
                                                 JsValueRef* arguments,
                                                 unsigned short argumentCount,
                                                 PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->fill(arguments, argumentCount);
    }
    JsValueRef fill(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_fillGradient;
    static JsValueRef CHAKRA_CALLBACK staticfillGradient(JsValueRef callee,
                                                         bool isConstructCall,
                                                         JsValueRef* arguments,
                                                         unsigned short argumentCount,
                                                         PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->fillGradient(arguments, argumentCount);
    }
    JsValueRef fillGradient(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_stroke;
    static JsValueRef CHAKRA_CALLBACK staticstroke(JsValueRef callee,
                                                   bool isConstructCall,
                                                   JsValueRef* arguments,
                                                   unsigned short argumentCount,
                                                   PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->stroke(arguments, argumentCount);
    }
    JsValueRef stroke(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_strokeGradient;
    static JsValueRef CHAKRA_CALLBACK staticstrokeGradient(JsValueRef callee,
                                                           bool isConstructCall,
                                                           JsValueRef* arguments,
                                                           unsigned short argumentCount,
                                                           PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->strokeGradient(arguments, argumentCount);
    }
    JsValueRef strokeGradient(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_setTransform;
    static JsValueRef CHAKRA_CALLBACK staticsetTransform(JsValueRef callee,
                                                         bool isConstructCall,
                                                         JsValueRef* arguments,
                                                         unsigned short argumentCount,
                                                         PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->setTransform(arguments, argumentCount);
    }
    JsValueRef setTransform(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_setLineStyle;
    static JsValueRef CHAKRA_CALLBACK staticsetLineStyle(JsValueRef callee,
                                                         bool isConstructCall,
                                                         JsValueRef* arguments,
                                                         unsigned short argumentCount,
                                                         PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->setLineStyle(arguments, argumentCount);
    }
    JsValueRef setLineStyle(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_setGlobalOpacity;
    static JsValueRef CHAKRA_CALLBACK staticsetGlobalOpacity(JsValueRef callee,
                                                             bool isConstructCall,
                                                             JsValueRef* arguments,
                                                             unsigned short argumentCount,
                                                             PVOID callbackData)
    {
        return reinterpret_cast<CanvasProjections*>(callbackData)->setGlobalOpacity(arguments, argumentCount);
    }
    JsValueRef setGlobalOpacity(JsValueRef* arguments, unsigned short argumentCount);
};
}  // namespace HoloJs
