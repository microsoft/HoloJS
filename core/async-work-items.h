#pragma once

#include "include/holojs/private/holojs-view.h"
#include "script-context.h"
#include <functional>
#include <memory>

namespace HoloJs {

class IHoloJsScriptHostInternal;

class BackgroundWorkItem : public IBackgroundWorkItem {
   public:
    BackgroundWorkItem(std::shared_ptr<HoloJs::ScriptContext> context, std::shared_ptr<std::function<long()>> workItem)
        : m_context(context), lambda(workItem)
    {
        m_context->asyncWorkerStarted();
    }

    virtual ~BackgroundWorkItem() { m_context->asyncWorkerExited(); }

    virtual long long getTag() { return 0; }

   private:
    virtual long execute() { return (*lambda.get())(); }
    std::shared_ptr<ScriptContext> m_context;
    std::shared_ptr<std::function<long()>> lambda;
};

class ForegroundWorkItem : public IForegroundWorkItem {
   public:
    ForegroundWorkItem(std::shared_ptr<ScriptContext> context, std::shared_ptr<std::function<void()>> workItem)
        : m_context(context), lambda(workItem)
    {
        m_context->asyncWorkerStarted();
    }

    virtual ~ForegroundWorkItem() { m_context->asyncWorkerExited(); }

    virtual void execute() { (*lambda.get())(); }

    virtual long long getTag() { return 0; }

   private:
    std::shared_ptr<ScriptContext> m_context;
    std::shared_ptr<std::function<void()>> lambda;
};

class BackgroundExecutionQueue {
   public:
    BackgroundExecutionQueue() {}
    ~BackgroundExecutionQueue() {}

    void asyncWorkerStarted() { m_asyncWorkerCount++; }
    void asyncWorkerExited() { m_asyncWorkerCount--; }

    bool isEmpty() { return m_asyncWorkerCount == 0; }

   private:
    std::atomic<unsigned int> m_asyncWorkerCount = 0;
};

class HostBackgroundWorkItem : public IBackgroundWorkItem {
   public:
    HostBackgroundWorkItem(std::shared_ptr<HoloJs::BackgroundExecutionQueue> context,
                           std::shared_ptr<std::function<long()>> workItem)
        : m_context(context), lambda(workItem)
    {
        m_context->asyncWorkerStarted();
    }

    virtual ~HostBackgroundWorkItem() { m_context->asyncWorkerExited(); }

    virtual long long getTag() { return 1; }

   private:
    virtual long execute() { return (*lambda.get())(); }
    std::shared_ptr<BackgroundExecutionQueue> m_context;
    std::shared_ptr<std::function<long()>> lambda;
};
}  // namespace HoloJs