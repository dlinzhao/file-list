#ifndef __FileList_H_
#define __FileList_H_

#include <vector>
#include <string>

namespace flst{
    class FileList
    {
    public:
        FileList(std::string _wkpath, std::string _extends);
        bool changeDir(std::string _wkpath, std::string _extends);
        bool nextBatch(std::vector<std::string> &img_name,
                       std::vector<int> &img_label, int batches);
        inline int  getNumofFiles(){ return files.size(); }

        int getFileList(std::string wkpath, std::string extends,
                         std::vector<std::pair<std::string, int> > &files);

    private:
        long long n_files;
        long long cur_pos;
        std::string wkpath;
        std::string extends;
        std::vector<std::pair<std::string, int> > files;
    };
}
#endif // FileList_H
