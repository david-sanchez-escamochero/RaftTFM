#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdint.h>

using namespace std;


class ManagerLog
{
public:
    ManagerLog();
    uint32_t write_log(std::string file_name, void* log, uint32_t size_to_write);
    uint32_t read_log(std::string file_name, void* log, uint32_t size_to_read);
};

