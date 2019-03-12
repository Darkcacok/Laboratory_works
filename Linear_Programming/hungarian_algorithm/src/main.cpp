#include <iostream>


#include "hungarian.h"




int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "usage: exe /path/to/file\n";

        return -1;
    }
    try{
        std::string f_path = std::string(argv[1]);
        Hungarian *test = new Hungarian(f_path);

        test->print();
    }
    catch(std::string str){
        std::cout << str;
    }
    return 0;
}