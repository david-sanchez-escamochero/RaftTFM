#pragma once

#include <string>
#include <mutex>

class Log
{
public:
	Log();
	static void trace(std::string str_log);
	
private:
	
	std::mutex mu_;
};

