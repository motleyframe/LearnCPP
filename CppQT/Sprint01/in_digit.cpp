#include <iostream>
#include <string>
using namespace std;

int main() {
    size_t counter = 0;
    std::string str;
    std::getline(cin,str);

    bool in_digit = false;
    for(size_t i = 0; i < str.length(); ++i) {
        if (isdigit(str[i])) {
            if (!in_digit) {
                ++counter;
                in_digit = true;
            }
        } else in_digit = false;
    }
    std::cout<<counter;

    return 0;
}
// "В эпичной битве сразились 40 витязей прекрасных, 300 спартанцев и 40 разбойников"s
