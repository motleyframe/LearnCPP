#include <cstdint>
#include <iostream>
#include <limits>

int main() {
    const auto min = std::numeric_limits<int64_t>::min();
    const auto max = std::numeric_limits<uint64_t>::max();

    std::cout<<std::hex<<min<<std::endl;
    std::cout<<std::hex<<max<<std::endl;
    std::cout<<std::hex<<min+max<<std::endl;
    std::cout<<std::hex<<2*min<<std::endl;
    std::cout<<std::hex<<max*2<<std::endl;

    return 0;
}
