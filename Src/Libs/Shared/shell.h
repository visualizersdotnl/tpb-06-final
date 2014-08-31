#pragma once

#include <string>

bool ExecuteProcessBlocking(
	const std::string& executablePath, 
	const std::string& args,
	std::string& output);