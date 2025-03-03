#include "error_handling.h"

void error_callback(int error, const char* description, char data)
{
    if (data != '\0')
        std::cerr << "Error: " << description << " " << data << std::endl;
    else
        std::cerr << "Error: " << description << std::endl;
}
