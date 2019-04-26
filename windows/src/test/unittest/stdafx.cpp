// stdafx.cpp : source file that includes just the standard includes
// libholojs-unittest.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

using namespace concurrency;

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

// Creates a task that completes after the specified delay.
task<void> complete_after(unsigned int timeout)
{
    // A task completion event that is set when a timer fires.
    task_completion_event<void> tce;

    // Create a non-repeating timer.
    auto fire_once = new timer<int>(timeout, 0, nullptr, false);
    // Create a call object that sets the completion event after the timer fires.
    auto callback = new call<int>([tce](int) { tce.set(); });

    // Connect the timer to the callback and start the timer.
    fire_once->link_target(callback);
    fire_once->start();

    // Create a task that completes after the completion event is set.
    task<void> event_set(tce);

    // Create a continuation task that cleans up resources and
    // and return that continuation task.
    return event_set.then([callback, fire_once]() {
        delete callback;
        delete fire_once;
    });
}
