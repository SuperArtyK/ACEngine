
#include <iostream>
#include "AEFrame.hpp"
int main()
{
    AEFrame myfr(1);
    while (1) {
        std::cout << "Hello ACEngine!;  ";
        myfr.sleep();
    }
}
