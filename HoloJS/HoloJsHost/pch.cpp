#include "pch.h"

void Log(PCSTR file, int line)
{
	std::string completeOutput = "Failure in file ";
	completeOutput += file;
	completeOutput += ", line ";
	completeOutput += std::to_string(line);
	completeOutput += "\r\n";
	OutputDebugStringA(completeOutput.c_str());
}
