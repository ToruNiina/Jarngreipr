#include <jarngreipr/jarngreipr.hpp>
#include <iostream>

int main(int argc, char **argv)
{
    //using namespace jarngreipr;
    if(argc != 2)
    {
        std::cerr << "Usage: jarngreipr [file.toml]" << std::endl;
        return 1;
    }

    const std::string fname(argv[1]);

    std::cout << "File: " << fname << std::endl;

    return 0;
}
