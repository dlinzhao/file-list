#include "filelist.h"

#include <iostream>

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        std::cerr << "Useage: " << argv[0] << "file_dir file_type[eg: .jpg]" << std::endl;
        return -1;
    }
    flst::FileList files(argv[1], argv[2]);
    std::vector<std::string> file_name;
    std::vector<int> file_label;
    int batches = 64;

    files.nextBatch(file_name, file_label, batches);

    for(int i = 0; i < batches; i++)
    {
        std::cout << "label:" << file_label[i] << "\tfile_name:" << file_name[i] << std::endl;
    }

    return 0;
}
