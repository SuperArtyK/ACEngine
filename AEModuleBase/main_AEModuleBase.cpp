
#include <iostream>
#include "include/AEModuleBase.hpp"

class Test3 : public __AEModuleBase<Test3> {

public:
    Test3(int s) {
        std::cout << "Constructing Test! "<<s<<"\n";
    }

};


REGISTER_CLASS(Test3);

int main() {
    Test3 a = Test3(3);
    
    std::cout << Test3::getModuleName()<<std::endl<<Test3::getModuleAmount();

    return 0;
}
