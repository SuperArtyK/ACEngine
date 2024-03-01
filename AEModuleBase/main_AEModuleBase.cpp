
#include <iostream>
#include "include/AEModuleBase.hpp"
using namespace std;



class Test3 : public __AEModuleBase<Test3> {

    

public:
    Test3(int s) {
        std::cout << "Constructing Test! "<<s<<"\n";
        num = s;
    }

    Test3& operator=(const Test3& val) {
        num = val.num;
        return *this;
    }

    int num = 0;

    REGISTER_MODULE(Test3)
};

//REGISTER_MODULE(Test3);

int main() {
    Test3 a = Test3(3);
    
    Test3 b(5);
    b = a;

    std::cout << Test3::getModuleName()<<std::endl<<Test3::getModuleAmount()<<endl;

    return 0;
}
