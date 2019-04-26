// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include <winerror.h>
#include <Windows.h>
#include "targetver.h"

// Headers for CppUnitTest
#include "CppUnitTest.h"

#include <agents.h>
#include <ppltasks.h>
concurrency::task<void> complete_after(unsigned int timeout);