#include <iostream>
#include <cmath>
#include <cstdint>
using namespace std::string_literals;

int main() {
    int n;
    std::cin>>n;

    if(n<0 || n>63) {
        std::cout<<"Wrong input\n"s;
        return 0;
    }

    switch(n) {
        case 0:
            std::cout<<"1"s;
            break;
        case 1:
            std::cout<<"2"s;
            break;
        default:
            uint64_t m = static_cast<uint64_t>(n);
            for(uint64_t i=1; i<=m; ++i) {
                std::cout<<(1U<<i)<<(i+1U<m ? ", "s : ""s);
            }
    }
    std::cout<<std::endl;

    return 0;
}
