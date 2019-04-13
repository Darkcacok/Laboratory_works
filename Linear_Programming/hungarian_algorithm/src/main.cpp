#include <iostream>


#include "hungarian.h"
#include "matrix.h"



int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "usage: exe /path/to/file\n";

        return -1;
    }
    try{
        std::string f_path = std::string(argv[1]);
        Matrix matrix(f_path);
        Hungarian *test = new Hungarian(matrix);

        std::cout << matrix << std::endl;
        test->solve();
        test->get_solve();
    }
    catch(std::string str){
        std::cout << str;
    }
    return 0;
}