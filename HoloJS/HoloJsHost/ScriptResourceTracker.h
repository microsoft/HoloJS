#pragma once

namespace HologramJS
{
	namespace Utilities
	{
		class ScriptResourceTracker
		{
		public:

			static void ReleaseAll();

			~ScriptResourceTracker()
			{
				ReleaseAll();
			}

			static JsErrorCode CreateAndTrackExternalBuffer(size_t bufferLength, void** allocatedBuffer, JsValueRef* scriptBuffer)
			{
				*allocatedBuffer = new unsigned char[bufferLength];
				return JsCreateExternalArrayBuffer(*allocatedBuffer, bufferLength, JsFinalizeArray, nullptr, scriptBuffer);
			}

			static void TrackObject(HologramJS::API::ExternalObject* object)
			{
				std::lock_guard<std::mutex> guard(ResourcesListLock);
				ProjectedResources.push_back(object);
			}

			static JsValueRef ObjectToDirectExternal(HologramJS::API::ExternalObject* object)
			{
				JsValueRef externalObjectRef;
				RETURN_INVALID_REF_IF_JS_ERROR(JsCreateExternalObject(object, JsFinalize, &externalObjectRef));
				TrackObject(object);

				return externalObjectRef;
			}

			template<typename T> static JsValueRef ObjectToDirectExternal(T* object)
			{
				if (object != nullptr)
				{
					HologramJS::API::ExternalObject* externalObject = new HologramJS::API::ExternalObject();
					RETURN_INVALID_REF_IF_FALSE(externalObject->Initialize(object));
					return ScriptResourceTracker::ObjectToDirectExternal(externalObject);
				}
				else
				{
					return JS_INVALID_REFERENCE;
				}
			}

			static JsValueRef ObjectToExternal(HologramJS::API::ExternalObject* object)
			{
				JsValueRef externalObject;
				RETURN_INVALID_REF_IF_JS_ERROR(JsCreateExternalObject(object, JsFinalize, &externalObject));

				JsValueRef wrapperObject;
				RETURN_INVALID_REF_IF_JS_ERROR(JsCreateObject(&wrapperObject));

				JsPropertyIdRef externalPropertyId;
				RETURN_INVALID_REF_IF_JS_ERROR(JsGetPropertyIdFromName(L"external", &externalPropertyId));

				RETURN_INVALID_REF_IF_JS_ERROR(JsSetProperty(wrapperObject, externalPropertyId, externalObject, true));

				TrackObject(object);

				return wrapperObject;
			}

			template<typename T> static JsValueRef ObjectToExternal(T* object)
			{
				if (object == nullptr)
				{
					return JS_INVALID_REFERENCE;
				}
				else
				{
					HologramJS::API::ExternalObject* externalObject = new HologramJS::API::ExternalObject();
					RETURN_INVALID_REF_IF_FALSE(externalObject->Initialize(object));
					return ScriptResourceTracker::ObjectToExternal(externalObject);
				}
			}

			static ElementWithEvents* ExternalToEventsInterface(JsValueRef value)
			{
				bool hasExternal;
				RETURN_NULL_IF_JS_ERROR(JsHasExternalData(value, &hasExternal));

				if (hasExternal)
				{
					HologramJS::API::ExternalObject* externalObject;
					RETURN_NULL_IF_JS_ERROR(JsGetExternalData(value, (void**)&externalObject));

					return externalObject->EventsInterface();
				}
				else
				{
					return nullptr;
				}
			}

			template<typename T> static T* ExternalToObject(JsValueRef value)
			{
				bool hasExternal;
				RETURN_NULL_IF_JS_ERROR(JsHasExternalData(value, &hasExternal));

				if (hasExternal)
				{
					HologramJS::API::ExternalObject* externalObject;
					RETURN_NULL_IF_JS_ERROR(JsGetExternalData(value, (void**)&externalObject));

					return reinterpret_cast<T*>(externalObject->Data());
				}
				else
				{
					return nullptr;
				}
			}

		private:
			static std::list<HologramJS::API::ExternalObject*> ProjectedResources;
			static std::mutex ResourcesListLock;

			static VOID CHAKRA_CALLBACK JsFinalize(_In_opt_ void *data);
			static VOID CHAKRA_CALLBACK JsFinalizeArray(_In_opt_ void *data)
			{
				if (data != nullptr)
				{
					delete[] data;
				}
			}
		};

	}
}