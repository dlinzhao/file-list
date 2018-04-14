#include <algorithm>
#include <iostream>
#include <random>
#include <cstring>
#include <cstdlib>
#include <cassert>

#if defined(_WIN32) || defined(_WIN64)
#include <io.h>
#else
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "filelist.h"

#define MAX_SIZE 1024

namespace flst {

    FileList::FileList(std::string _wkpath, std::string _extends)
    {
        changeDir(_wkpath, _extends);
    }

    bool FileList::changeDir(std::string _wkpath, std::string _extends)
    {
        bool ok = false;
        if(_wkpath.empty())
            return ok;

        this->wkpath = _wkpath;
        this->extends = _extends;

        n_files = 0;
        cur_pos = 0;
        files.clear();

        int ret = getFileList(wkpath, extends, files);

        if(ret == 0)
        {
            ok = true;
            n_files = files.size();
        }

        return ok;
    }

    bool FileList::nextBatch(std::vector<std::string> &img_name, std::vector<int> &img_label, int batches)
    {
        if(n_files == 0)
            return false;

        std::random_device rd;
        std::mt19937 g(rd());

        for(int i = 0; i < batches && i < n_files; i++)
        {
            img_name.push_back(files[cur_pos].first);
            img_label.push_back(files[cur_pos].second);
            cur_pos = (++cur_pos) % n_files;

            if(cur_pos == 0)
                std::shuffle(files.begin(), files.end(), g);
        }

        return true;
    }

    int FileList::getFileList(std::string wkpath, std::string extends, std::vector<std::pair<std::string, int>> &files)
    {
        std::random_device rd;
        std::mt19937 g(rd());

#if defined(_WIN32) || defined(_WIN64)
        // ================  windows  ==============
        _finddata_t fileInfo;

        char name[MAX_SIZE];
        memset(name, 0, sizeof(name));
        strcpy(name, wkpath.c_str());
        strcat(name, "\\*");

        intptr_t handle = _findfirst(name, &fileInfo);

        if(handle == -1L)
        {
            std::cerr << "walkDir: open dir failed! " << wkpath << std::endl;
            return -1;
        }

        int pre = -1;
        std::string _extends;
        std::string fpath(wkpath);
        std::string prefix;

        if(fpath[fpath.length() - 1] != '\\')
            fpath.append("\\");

        do
        {
            // subdir
            if(fileInfo.attrib & _A_SUBDIR)
            {
                //if(strcmp(fileInfo.name, ".") == 0 || strcmp(fileInfo.name, "..") == 0)
                continue;
            }
            else
            {
                std::string file(fileInfo.name);

                _extends = file.substr(file.length() - 4);

                if(extends != _extends)
                    continue;

                prefix = file.substr(0,file.find_first_of('_'));
                pre = atoi(prefix.c_str());

                files.push_back(std::pair<std::string, int>(fpath + file, pre - 1));
            }
        }while(_findnext(handle, &fileInfo) == 0);

        _findclose(handle);
#else
        // ================  unix and linux  ==============
        struct dirent *dp;
        DIR *dfd;

        if((dfd = opendir(wkpath.c_str())) == NULL)
        {
            return -1;
        }

        int pre = -1;
        std::string _extends;
        std::string fpath(wkpath);
        std::string prefix;

        if(fpath[fpath.length() - 1] != '/')
            fpath.append("/");

        while((dp = readdir(dfd)) != NULL)
        {
            if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
                continue;
            // files
            std::string file(dp->d_name);

            _extends = file.substr(file.length() - 4);

            if(extends != _extends)
                continue;

            prefix = file.substr(0,file.find_first_of('_'));
            pre = atoi(prefix.c_str());

            files.push_back(std::pair<std::string, int>(fpath + file, pre - 1));
        }
        closedir(dfd);
#endif
        // shuffle
        std::shuffle(files.begin(), files.end(), g);

        return 0;
    }

}
