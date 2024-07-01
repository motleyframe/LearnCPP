//Задача про избирательные бюллетени.
//Решение - в одну строку, т.к. pair сортируется лексикографически по 1-му эл-ту пары.
//Если по 1-му сортировка невозможна, то сортируется по 2-му
//разворот делается с использованием итераторов rbegin(), rend() прямо внутри sort()

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std::string_literals;

typedef std::pair<int,std::string> Person;

int main() {
    int count;
    std::cin>>count;

    if(count > 1) {
        std::vector<Person> Deputies;

        for(int i=0;i<count;++i) {
            std::string name;
            int age;
            std::cin>>name>>age;
            Deputies.push_back({name,age});
        }

        std::sort(Deputies.rbegin(),Deputies.rend());

        for(const auto& p : Deputies) 
            std::cout<<p.first<<std::endl;
    } else {		//Если ввели всего одну фамилию, то не надо выделять память под вектор
        std::string name;
        int age;
        std::cin>>name>>age;
        std::cout<<name<<std::endl;
    }
    return 0;
}
