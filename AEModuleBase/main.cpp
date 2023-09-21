
#include <iostream>
#include "AEModuleBase.hpp"

class Test : public __AEBasicModule<Test> {

public:
    Test(int s) {
        std::cout << "Constructing Test! "<<s<<"\n";
    }

};


REGISTER_CLASS(Test);

int main() {
    Test a = Test(3);
    
    std::cout << Test::getModuleName()<<std::endl<<Test::getModuleAmount();

    return 0;
}
