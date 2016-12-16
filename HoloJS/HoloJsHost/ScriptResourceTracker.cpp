#include "pch.h"
#include "ScriptResourceTracker.h"

using namespace HologramJS::Utilities;

std::list<HologramJS::API::ExternalObject*> ScriptResourceTracker::ProjectedResources;
std::mutex ScriptResourceTracker::ResourcesListLock;


_Use_decl_annotations_
void
CHAKRA_CALLBACK
ScriptResourceTracker::JsFinalize(void *data)
{
	HologramJS::API::ExternalObject* iRelease = reinterpret_cast<HologramJS::API::ExternalObject*>(data);
	if (iRelease == nullptr)
	{
		return;
	}

	std::lock_guard<std::mutex> guard(ResourcesListLock);

	for (auto& object : ProjectedResources)
	{
		if (object == iRelease)
		{
			delete iRelease;
			ProjectedResources.remove(object);
			break;
		}
	}
}

void
ScriptResourceTracker::ReleaseAll()
{
	std::lock_guard<std::mutex> guard(ResourcesListLock);

	for (auto& object : ProjectedResources)
	{
		if (object != nullptr)
		{
			delete object;
		}
	}

	ProjectedResources.clear();
}