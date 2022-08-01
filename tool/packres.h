#pragma once

namespace packres{
    /**
     * @brief Output miniopt library files to the directory
     * 
     * @param dir[in]   Specify a directory.
     * 
     * @return 0        Pass.
     * @return other    Fail.
     */
    int output(const char* dir);
}