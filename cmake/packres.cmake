#[[
    cmake -DIN_FILE_LIST=<input-file-list>
          -DOUT=<output-file-name>
          -P packres.cmake
]]


set(CodePrefix
"// Generated code.

#include <fstream>
#include <iostream>

namespace packres{
    struct File{
        File(const char* name_, const char* data_) :name(name_), data(data_){}
        const char* name;
        const char* data;
    };
    // Export all resources(write them to files).
    int output(const char* dir){
        int status = 0;
        const File fileArray[]={
"
)

set(CodeSuffix
"        };
        for(auto& file : fileArray){
            std::string fullName;
            if(dir){
                fullName.assign(dir);
                if(fullName.back() != '/') fullName += \"/\";
            }
            fullName += file.name;

            std::ofstream ofs(fullName);
            if(ofs){
                ofs << file.data;
                ofs.flush();
                ofs.close();
            }else{
                printf(\"error: cannot write file = [%s]\\n\", fullName.c_str());
                status = -1;
                break;
            }
        }
        return status;
    } // output
} // namespace packres
"
)

if(NOT IN_FILE_LIST)
    message(FATAL_ERROR "input file list cannot be empty.")
endif()

if(NOT OUT)
    message(FATAL_ERROR "output file name cannot be empty.")
endif()

if(IN_FILE_LIST)
    set(IsFirstTime "TRUE")
    file(WRITE ${OUT} "${CodePrefix}")

    foreach(file ${IN_FILE_LIST})
        file(READ ${file} fileBuf)
        get_filename_component(shortName ${file} NAME)

        if(IsFirstTime)
            set(IsFirstTime "FALSE")
        else()
            file(APPEND ${OUT} ",\n")
        endif()

        file(APPEND ${OUT} "{\"" "${shortName}" "\", R\"--(" "${fileBuf}"  ")--\"}")
            
    endforeach()

    file(APPEND ${OUT} "${CodeSuffix}")
endif()