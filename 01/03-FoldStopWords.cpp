/* Напишите функцию AddStopWords, которая принимает строку-запрос и множество стоп-слов и
 * возвращает новую строку. Новая строка состоит из запроса, к которому приписаны стоп-слова.
 * Стоп-слова разделяются пробелами, и перед каждым стоит дефис.
Для решения задачи не используйте циклы, их заменит алгоритм accumulate.
 */


#include <iostream>
#include <string>
#include <numeric>
#include <algorithm>
#include <set>

using namespace std::string_literals;

std::string AddStopWords(const std::string& query, const std::set<std::string>& stop_words) {

    return stop_words.empty() ? query
                              : query + std::accumulate( stop_words.cbegin(),stop_words.cend(),std::string(),[]
                                            ( const std::string& left,const std::string& right ) {
                                              return left + " -"s + right;
                                            }
                                );
}

std::string Prompt() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

int main() {
    const std::string query = Prompt();
    const std::set<std::string> stop_words = {"of"s,"in"s};

    std::cout<<AddStopWords(query,stop_words)<<std::endl;

    return 0;
}
