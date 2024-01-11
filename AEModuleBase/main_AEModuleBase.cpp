
#include <iostream>
#include "include/AEModuleBase.hpp"
using namespace std;



class Test3 : public __AEModuleBase<Test3> {

public:
    Test3(int s) {
        std::cout << "Constructing Test! "<<s<<"\n";
    }

};


REGISTER_MODULE(Test3);

int main() {
    Test3 a = Test3(3);
    
    std::cout << Test3::getModuleName()<<std::endl<<Test3::getModuleAmount()<<endl;

    return 0;
}
