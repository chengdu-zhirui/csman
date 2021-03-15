/*
 * CovScript Manager: OS Depended Functions Implementation
 * Licensed under Apache 2.0
 * Copyright (C) 2020-2021 Chengdu Covariant Technologies Co., LTD.
 * Website: https://covariant.cn/
 * Github:  https://github.com/chengdu-zhirui/
 *
 * refer to https://blog.csdn.net/venom_snake/article/details/88066475
 */
#include <csman/global.hpp>
#include <iostream>
#include <cstring>
#include <cstdio>

#ifdef MOZART_PLATFORM_WIN32

#include <windows.h>
#include <direct.h>        //for mkdir rmdir
#include <io.h>            //for access

#elif MOZART_PLATFORM_UNIX
#include <sys/stat.h>	//for access
#include <unistd.h>		//for mkdir rmdir
#include <dirent.h>		//for DIR remove
#endif

#ifdef MOZART_PLATFORM_WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#define RMDIR(a) _rmdir((a))
#elif MOZART_PLATFORM_UNIX
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#define RMDIR(a) rmdir((a))
#endif

bool csman::sys::exist(const std::string &path) {
    return ACCESS(path.c_str(), 0) == 0;
}

namespace csman::sys::file{
    bool create(std::string path){
        path = csman::str::to_file_path(path);
        std::ofstream ofs;
        for(int i = path.size()-1; i>=0 ; i--)
            if(path[i]==delimiter){
                sys::dir::create(path.substr(0,i));
                ofs.open(path);
                break;
            }
        ofs.close();
        ofs.open(path);
        if(!ofs.is_open()){
            ofs.close();
            return false;
        }
        ofs.close();
        return true;
    }
    bool remove(std::string path) {
        path = csman::str::to_file_path(path);
        return ::remove(path.c_str())==0;
    }
}

namespace csman::sys::dir {
    bool create(std::string path)   // 存在则不建立
    {
        path = csman::str::to_dir_path(path);
        for (int i = 0; i < path.size(); i++) {
            if (path[i] != '/' && path[i] != '\\')
                continue;
            std::string tmp = path.substr(0, i + 1).c_str();
            int status = ACCESS(tmp.c_str(), 0);
            if (status != 0) {
                status = MKDIR(tmp.c_str());
                if (status != 0)
                    return false;
            }
        }
        return true;
    }
    bool remove(std::string path) {
        std::string strPath = csman::str::to_dir_path(path);;
#ifdef MOZART_PLATFORM_WIN32
        struct _finddata_t fb;
        if (strPath.at(strPath.length() - 1) != '\\' || strPath.at(strPath.length() - 1) != '/')
            strPath.append("\\");
        std::string findPath = strPath + "*";
        intptr_t handle;
        handle = _findfirst(findPath.c_str(), &fb);
        if (handle != -1L) {
            std::string pathTemp;
            do {
                if (strcmp(fb.name, "..") != 0 && strcmp(fb.name, ".") != 0) {
                    pathTemp.clear();
                    pathTemp = strPath + std::string(fb.name);
                    if (fb.attrib == _A_SUBDIR)//_A_SUBDIR=16 it is dir
                        sys::dir::remove(pathTemp.c_str());
                    else
                        ::remove(pathTemp.c_str());
                }
            } while (0 == _findnext(handle, &fb));
            _findclose(handle);
        }
        return ::RMDIR(strPath.c_str()) == 0;

#elif MOZART_PLATFORM_UNIX
        if (strPath.at(strPath.length() - 1) != '\\' || strPath.at(strPath.length() - 1) != '/')
        strPath.append("/");
        DIR *d = opendir(strPath.c_str());
        if (d != NULL) {
            struct dirent *dt = NULL;
            while (dt = readdir(d)) {
                if (std::strcmp(dt->d_name, "..")!=0 && strcmp(dt->d_name, ".")!=0) {
                    struct stat st;
                    std::string fileName;
                    fileName = strPath + std::string(dt->d_name);
                    stat(fileName.c_str(), &st);
                    if (S_ISDIR(st.st_mode))
                        sys::dir::remove(fileName);
                    else
                        ::remove(fileName.c_str());
                }
            }
            closedir(d);
        }
        return ::RMDIR(strPath.c_str()) == 0;
#endif
    }
}

