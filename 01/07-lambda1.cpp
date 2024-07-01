/*Потренируйтесь в работе с более сложными лямбда-функциями и напишите программу, которая считывает из стандартного ввода неотрицательное целое число N, за которым следуют N целых чисел, разделённых одним пробелом. Эти числа должны быть отсортированы в следующем порядке:
сначала должны идти чётные числа по возрастанию,
затем нечётные числа по убыванию.
Отсортированные числа выведите в стандартный поток. После каждого числа, включая последнее, должен быть выведен один пробельный символ. Если число N равно нулю, программа не должна считывать другие числа и выводить что-либо в поток вывода.
Код ввода и вывода чисел уже дан в заготовке. Напишите тело лямбда-функции, переданной в алгоритм sort.
*/

#include <iostream>
#include <set>
#include <string>
#include <iterator>
#include <algorithm>
#include <vector>

using namespace std::string_literals;

template<typename T>
std::ostream& operator<<(std::ostream& s, std::vector<T> t) {
    for (std::size_t i = 0; i < t.size(); ++i)
        s << t[i] << (i == t.size() - 1 ? std::string() : " "s);
    return s << std::endl;
}

int main() {
    unsigned int N;
    std::cin>>N;

    std::vector<int> values;
    for(int i=0;i<N;++i) {
        int k;
        std::cin>>k;
        values.push_back(k);
    }

    sort(values.begin(),values.end(),[]
        (const int& a, const int& b) {
            const bool a_even = (a%2==0);
            const bool b_even = (b%2==0);

            if(a_even && b_even) {
                return a<b;
            } else if(a_even && !b_even) {
                return true;
            } else if(!a_even && b_even) {
                    return false;
            } else return a>b;
        }
    );

    std::cout<<values;

    return 0;
}
