#include <iostream>
#include <string>
using namespace std::string_literals;

std::string parse_string(const std::string& str) {
    std::string str_copy;

    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i]=='\\' && i!=str.size()-1) {                   // признак экранирования. Убедимся, что это не последний символ строки
            switch(str[i+1]) {                                // символ после бэкслеша
                case '\\':
                    str_copy += '\\';                           // просто добавляем \ в str_copy
                    break;
                case '\"':
                    str_copy += '\"';                           // просто добавляем " в str_copy
                    break;
                case 'n':
                    str_copy += '\n';                           // литерал перевода строки
                    break;
                case '0':  return str_copy;                     // как таковой обрезки нет, просто возвращаем str_copy
                default:                                        // все остальные бекслеши
                    str_copy += '\\';                           // оставляем
                    str_copy += str[i+1];                     // конкатенируем следующий после бекслеша символ
            }
            ++i;                                // увеличиваем индекс после замены
        } else str_copy += str[i];              // если ничего интересного - просто склеиваем
    }
    return str_copy;
}

// Реализация посложнее: кавычки могут находиться в произвольном месте
void remove_quotes(std::string& str) {          // Убрать передние и задние кавычки, если обе имеются
    size_t first=0, last=str.size()-1;
    while (first<str.size() && std::isspace(str[first]))    // пройтись с начала строки, учитывая неотображаемые символы
        ++first;                                             // запомнить позицию

    while(last>0 && std::isspace(str[last]))                // пройтись с конца строки, учитывая неотображаемые символы
        --last;                                              // запомнить позицию
                                                             // если first и last - это не одна и та же кавычка
    if(first != last && str[last]=='\"' && str[first]=='\"') {
        str.erase(first,1);                                  // удаляем первую
        str.erase(last-1,1);                                 // удаляем вторую с поправкой на её новую позицию
    }
}

// Реализация попроще: кавычки ожидаются только в первой и последней позициях
void remove_quotes1(std::string& str) {
    if (str.length() >= 2 && str.front() == '\"' && str.back() == '\"') {
        str.erase(0, 1);
        str.pop_back();
    }
}

int main() {
    std::string line;
    std::getline(std::cin, line);

    if(!line.empty()) {							// с пустыми строками работать не будем
        remove_quotes(line);
        const std::string parsed_str=parse_string(line);
		std::cout<<parse_string(line);
    }
    std::cout<<std::endl;

    return 0;
}

