#include "ManagerLog.h"
#include "Tracer.h"

#define MANAGER_NO_ERROR                0
#define MANAGER_ERROR_TO_OPEN_FILE      1
#define MANAGER_ERROR_NOT_OPENED_FILE   2
#define MANAGER_ERROR_NOT_READ_LOG      3
#define MANAGER_ERROR_NOT_WRITE_LOG     4


ManagerLog::ManagerLog()
{

}

uint32_t ManagerLog::write_log(std::string file_name, void* log, uint32_t size_to_write)
{
    uint32_t ret = MANAGER_NO_ERROR;
    ofstream rlog(file_name, ios::out | ios::binary);
    if (!rlog) {
        Tracer::trace("ManagerLog::write_log - FAILED!!! Cannot open file: " + file_name + ".\r\n");
        ret = MANAGER_ERROR_TO_OPEN_FILE;
    }
    else {
        rlog.write((char*)log, size_to_write);
     
        if ( (rlog.bad()) || (rlog.fail()) )
            Tracer::trace("All characters from log were written successfully to " + file_name + "\r\n");
        else {
            Tracer::trace("ManagerLog::write_log - FAILED!!! to write, error " + std::to_string(rlog.rdstate()) + " \r\n");
            ret = MANAGER_ERROR_NOT_WRITE_LOG;
        }
        rlog.close();
    }
    return ret;
}

uint32_t ManagerLog::read_log(std::string file_name, void* log, uint32_t size_to_read)
{
    uint32_t ret = MANAGER_NO_ERROR;
    ifstream rlog(file_name, ios::in | ios::binary);
    if (!rlog) {
        Tracer::trace("ManagerLog::read_log - FAILED!!! Cannot open file: " + file_name + ".\r\n");
        ret = MANAGER_ERROR_TO_OPEN_FILE;
    }
    else {

        rlog.read((char*)log, size_to_read);

        if ((rlog.bad()) || (rlog.fail()))
            Tracer::trace("All characters from " + file_name + " were read successfully\r\n");
        else {
            Tracer::trace("ManagerLog::read_log - FAILED!!! Error: only " + std::to_string(rlog.gcount()) + " could be read\r\n");
            ret = MANAGER_ERROR_NOT_READ_LOG;
        }
        rlog.close();
    }
    return ret;
}

