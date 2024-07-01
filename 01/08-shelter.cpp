#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <set>
#include <iostream>

using namespace std::string_literals;

int CountAndAddNewDogs(const std::vector<std::string>& new_dogs,
                       const std::map<std::string, int>& max_amount,
                       std::map<std::string, int>& shelter) {


    return std::count_if(new_dogs.cbegin(),new_dogs.cend(),[&max_amount,&shelter]
        ( const std::string& breed ) {
            int& num = shelter[breed];

            if(num < max_amount.at(breed)) {
                ++num;
                return true;
            } else return false;
        }
    );
}

int main() {
    std::map<std::string, int> shelter = {{"landseer"s, 1}, {"otterhound"s, 2}, {"pekingese"s, 2}, {"pointer"s, 3}};
    const std::map<std::string, int> max_amount = {{"landseer"s, 2}, {"otterhound"s, 3}, {"pekingese"s, 4}, {"pointer"s, 7}};

    const std::vector<std::string> new_dogs = {"landseer"s, "otterhound"s, "otterhound"s, "otterhound"s, "pointer"s};

    std::cout << CountAndAddNewDogs(new_dogs, max_amount, shelter) << std::endl;

/*    std::map<std::string, int> shelter {
        {"shepherd"s, 1},
        {"corgi"s, 3},
    };

    std::map<std::string, int> max_amount {
        {"shepherd"s, 2},
        {"corgi"s, 3},
        {"shiba inu"s, 1},
    };

    std::cout << CountAndAddNewDogs({"shepherd"s, "shiba inu"s, "shiba inu"s, "corgi"s}, max_amount, shelter) << std::endl;*/
    return 0;
}
