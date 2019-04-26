#include "surface-mapping-projection.h"
#include "blob.h"
#include "include/holojs/private/error-handling.h"
#include "include/holojs/private/platform-interfaces.h"
#include "include/holojs/private/script-host-utilities.h"
#include "include/holojs/private/surface-mapping.h"
#include "resource-management/external-object.h"

#include <string>
#include <vector>

using namespace HoloJs;
using namespace HoloJs::ResourceManagement;
using namespace std;

long SurfaceMappingProjection::initialize()
{
    JS_PROJECTION_REGISTER(L"SurfaceMapper", L"isAvailable", isAvailable);
    JS_PROJECTION_REGISTER(L"SurfaceMapper", L"create", create);
    JS_PROJECTION_REGISTER(L"SurfaceMapper", L"start", start);
    JS_PROJECTION_REGISTER(L"SurfaceMapper", L"stop", stop);
    JS_PROJECTION_REGISTER(L"SurfaceMapper", L"triangleDensity", triangleDensity);
    JS_PROJECTION_REGISTER(L"SurfaceMapper", L"boundingCube", boundingCube);

    return HoloJs::Success;
}

JsValueRef SurfaceMappingProjection::_isAvailable(JsValueRef* arguments, unsigned short argumentCount)
{
    auto surfaceMapperAvailable = getPlatform()->isSurfaceMappingAvailable();
    JsValueRef surfaceMapperAvailableRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsBoolToBoolean(surfaceMapperAvailable, &surfaceMapperAvailableRef));

    return surfaceMapperAvailableRef;
}

JsValueRef SurfaceMappingProjection::_create(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto newSurfaceMapper = getPlatform()->getSurfaceMapper(m_host);
    RETURN_INVALID_REF_IF_NULL(newSurfaceMapper);

    auto surfaceMapperExternal =
        m_resourceManager->objectToDirectExternal(newSurfaceMapper, ObjectType::ISurfaceMapper);

    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(arguments[1], L"native", surfaceMapperExternal));

    return JS_INVALID_REFERENCE;
}

JsValueRef SurfaceMappingProjection::_start(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto surfaceMapper = m_resourceManager->externalToObject<ISurfaceMapper>(arguments[1], ObjectType::ISurfaceMapper);
    RETURN_INVALID_REF_IF_NULL(surfaceMapper);

    surfaceMapper->start(arguments[1]);

    return JS_INVALID_REFERENCE;
}

JsValueRef SurfaceMappingProjection::_stop(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto surfaceMapper = m_resourceManager->externalToObject<ISurfaceMapper>(arguments[1], ObjectType::ISurfaceMapper);
    RETURN_INVALID_REF_IF_NULL(surfaceMapper);

    surfaceMapper->stop();

    return JS_INVALID_REFERENCE;
}

JsValueRef SurfaceMappingProjection::_triangleDensity(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount >= 2);

    auto surfaceMapper = m_resourceManager->externalToObject<ISurfaceMapper>(arguments[1], ObjectType::ISurfaceMapper);
    RETURN_INVALID_REF_IF_NULL(surfaceMapper);

    if (argumentCount == 2) {
        float triangleDensity;
        surfaceMapper->getTriangleDensity(&triangleDensity);

        JsValueRef triangleDensityRef;
        RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(static_cast<double>(triangleDensity), &triangleDensityRef));
        return triangleDensityRef;
    } else {
        auto triangleDensity = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
        RETURN_INVALID_REF_IF_TRUE(triangleDensity <= 0);

        surfaceMapper->setTriangleDensity(triangleDensity);
        return JS_INVALID_REFERENCE;
    }
}

JsValueRef SurfaceMappingProjection::_boundingCube(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount >= 2);

    auto surfaceMapper = m_resourceManager->externalToObject<ISurfaceMapper>(arguments[1], ObjectType::ISurfaceMapper);
    RETURN_INVALID_REF_IF_NULL(surfaceMapper);

    if (argumentCount == 2) {
        JsValueRef cubeDataRef;
        unsigned char* cubeDataByteBuffer;
        RETURN_INVALID_REF_IF_FAILED(
            ScriptHostUtilities::CreateTypedArray(JsArrayTypeFloat32, &cubeDataRef, 6, &cubeDataByteBuffer));
        float* cubeBuffer = reinterpret_cast<float*>(cubeDataByteBuffer);

        surfaceMapper->getBoundingCube(
            &cubeBuffer[0], &cubeBuffer[1], &cubeBuffer[2], &cubeBuffer[3], &cubeBuffer[4], &cubeBuffer[5]);

        return cubeDataRef;
    } else {
        unsigned int elementCount;
        unsigned char* byteBuffer;
        RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetTypedArrayBufferFromRef(JsArrayTypeFloat32, arguments[2], &elementCount, &byteBuffer));
        RETURN_INVALID_REF_IF_FALSE(elementCount == 6);

        float* cubeBuffer = reinterpret_cast<float*>(byteBuffer);

        surfaceMapper->setBoundingCube(
            cubeBuffer[0], cubeBuffer[1], cubeBuffer[2], cubeBuffer[3], cubeBuffer[4], cubeBuffer[5]);

        return JS_INVALID_REFERENCE;
    }
}