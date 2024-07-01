/* В стандартном потоке дана одна строка, которая состоит из числа N и следующего за ним набора S из N слов. Число отделено от слов пробелом.
Отсортируйте набор слов S в алфавитном порядке по возрастанию, игнорируя регистр букв. Выведите их в стандартный поток вывода через пробел.

При сравнении слов цифры должны предшествовать буквам: слово «R2D2» находится после слова «R259», так как цифра 5 предшествует любой из букв.
После последнего слова также выведите пробел, а затем перевод строки.
Ознакомьтесь с функцией tolower и алгоритмом lexicographical_compare, чтобы отсортировать слова в алфавитном порядке без учета регистра символов.



 */

#include <iostream>
#include <cctype>
#include <vector>
#include <algorithm>
using namespace std::string_literals;



int main() {
    int N;
    std::cin>>N;

    std::vector<std::string> data;

    for(int i=0; i<N; ++i) {
        std::string s;
        std::cin>>s;
        data.push_back(s);
    }

    sort(data.begin(),data.end(),[]
        (const auto& lhs, const auto& rhs) {
            return lexicographical_compare(lhs.begin(),lhs.end(),
                                           rhs.begin(),rhs.end(),
                                           [](const char& left,const char& right) {
                                               return tolower(left)<tolower(right);
                                           });
        }
    );

    for(const auto& x : data)
        std::cout<<x<<' ';
    std::cout<<std::endl;

    return 0;
}
