#include <iostream>
#include <cmath>
#include <cstdint>
using namespace std::string_literals;

int main() {
    int n;
    std::cin>>n;

    double acc = .0;
    for(int i=0;i<n;++i) {
        int num;
        std::cin>>num;
        acc += num;
    }

    std::cout<<acc/n<<std::endl;

    return 0;
}
