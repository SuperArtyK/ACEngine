
#include <iostream>
#include "AEFrame.hpp"

int main()
{
    AEFrame a(60);
    a.setFps(5);
    AEFrame myfr = a;
    std::cout << myfr.getModuleAmount() << " " << myfr.getModuleName() << '\n';
    while (1) {
        std::cout << "Hello ACEngine!;  ";
        std::cout << myfr.getDelay() << '\n';
        myfr.sleep();
    }
}
