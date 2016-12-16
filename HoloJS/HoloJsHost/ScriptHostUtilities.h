#pragma once
namespace HologramJS
{
	namespace Utilities
	{
		class ScriptHostUtilities
		{
		public:
			ScriptHostUtilities();
			~ScriptHostUtilities();

			static bool ProjectFunction(
				const std::wstring& name,
				const std::wstring& namespaceName,
				JsNativeFunction function,
				void* callbackState,
				JsValueRef* scriptFunction
			);

			static bool ProjectFunction(
				const std::wstring& name,
				const std::wstring& namespaceName,
				JsNativeFunction function
			)
			{
				JsValueRef unusedScriptFunction;
				return ProjectFunction(name, namespaceName, function, nullptr, &unusedScriptFunction);
			}

			static bool GetJsProperty(JsValueRef& parentObject, const std::wstring& name, JsValueRef* createdProperty);

			static bool GetString(JsValueRef value, std::wstring& outString);

			static GLclampf GLclampfFromJsRef(JsValueRef value);

			static GLenum GLenumFromJsRef(JsValueRef value);

			static GLint GLintFromJsRef(JsValueRef value);

			static GLboolean GLbooleanFromJsRef(JsValueRef value);

			static GLbitfield GLbitfieldFromJsRef(JsValueRef value);

			static GLsizeiptr GLsizeiptrFromJsRef(JsValueRef value);

			static GLuint GLuintFromJsRef(JsValueRef value);

			static GLsizei GLsizeiFromJsRef(JsValueRef value);

			static GLfloat GLfloatFromJsRef(JsValueRef value);
		};

	}
}

