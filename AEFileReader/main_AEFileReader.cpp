#include <iostream>
#include "AEFileReader.hpp"
using namespace std;

void a(const std::string_view b) {
    cout << b.size() << " \"" << b.data() << "\"\n";
}



int main(){
    a({"12345", 4});
}
