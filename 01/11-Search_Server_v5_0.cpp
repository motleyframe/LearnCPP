/* Теперь недостаточно для каждого слова хранить список документов, в которых оно встречается.
 * Вместе с каждым документом нужно хранить TF соответствующего слова в этом документе. Эту проблему можно решить,
 * если заменить словарь «слово → документы» на более сложную структуру map<string, map<int, double>> word_to_document_freqs_,
 * которая сопоставляет каждому слову словарь «документ → TF».
При добавлении документа нужно вычислить TF каждого входящего слова. Это нужно делать в методе AddDocument.
Можно пройтись циклом по всему документу, увеличивая TF каждого встречаемого слова на величину 1 / N, где N — общее количество слов в документе. Если слово встретилось K раз, то вы столько же раз прибавите к его TF величину 1 / N и получите K / N.

TF вычисляется при добавлении документа, IDF — при поиске по запросу.
Подумайте, как с имеющимися данными быстро вычислять IDF.
При делении чисел убедитесь, что они представлены типом double, иначе произойдёт округление.
Используйте литерал с точкой. Например, 1. / 2 == 0.5, а 1 / 2 == 0.
Если понадобится преобразовать целое число к double, то можно использовать static_cast.
Как и раньше, для поиска документов и вычисления их релевантности хватит двойной вложенности циклов
for: внешний — по словам запроса, внутренний — по документам, где это слово встречается.
Будьте внимательны при работе со словарями. Не забывайте, что с константным словарём нельзя
испльзовать операцию [], а при использованни метода at обязательно проверять наличие ключей.
Всё сказанное относится как к самому словарю word_to_document_freqs_,
так и к каждому словарю, хранящемуся в нём. */

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <map>
using namespace std::literals;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

// Внешняя база-словарь, в которой сопоставлены плюс-слова и id документов,
// где эти слова есть, но притом уже вычищены минус-слова после сопоставления
// документов и запроса пользователя
// Используется в ф-ии GetDocsMap() и в Calculate_IDF()
std::map<std::string,std::set<int>> documents_map;

std::string ReadLine() {
    std::string s;
    std::getline(std::cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    std::cin >> result;
    ReadLine();
    return result;
}

std::vector<std::string> SplitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else
            word += c;
    }
    if (!word.empty())
        words.push_back(word);

    return words;
}

struct Document {
    int id=0;
    double relevance=0.0;
};

class SearchServer {
private:
    std::map<std::string,std::set<int>> documents_;         // search index
    std::set<std::string> stop_words_;
    int document_count_ = 0;
    std::map<std::string,
             std::map<int,double>> word_to_document_freqs_;    // "WORD -> (Doc#,TF)"

    struct QueryWord {
        std::string data;
        bool is_minus;
        bool is_stop;
    };

    struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

    inline bool IsStopWord(const std::string& w) const {
        return stop_words_.count(w) ;
    }

    std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const {
        std::vector<std::string> words;

        for (const std::string& word : SplitIntoWords(text))
            if (!IsStopWord(word))
                words.push_back(word);
        return words;
    }

    QueryWord ParseQueryWord(std::string text) const {
        bool is_minus = false;
        // Word shouldn't be empty
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        return {text, is_minus, IsStopWord(text)};
    }

    Query ParseQuery(const std::string& text) const {
        Query query;

        for (const std::string& w : SplitIntoWords(text)) {
            const QueryWord query_word = ParseQueryWord(w);

            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                } else
                    query.plus_words.insert(query_word.data);
            }
        }
        return query;
    }

    /*Получить словарь, в котором сопоставлены плюс-слова и id документов,
     *где они есть, но притом нет минус-слов*/
    void SetDocsMap(const Query& query) const {
        std::map<std::string,std::set<int>> docs_map;        // сюда запишем результат работы
        /*1. Пробегаемся по плюс-словам, проверяем, есть ли каждое из этих слов
         в индексе
          2. Добавляем в docs_set id документов, где встречаются эти плюс-слова
        */

        for(const auto& p : query.plus_words) {
            try {                                           //try-catch чтобы отлавливать исключение
                const auto& docs_plus_set = documents_.at(p);   //вследствие обращения по несуществующему ключу
                docs_map[p].insert(docs_plus_set.cbegin(),
                                   docs_plus_set.cend()); //запишем эти id в словарь
            } catch (std::out_of_range& e) {}
        }

        /* Пробегаемся по минус-словам, проверяем, есть ли каждое из этих слов в индексе */

        if(!query.minus_words.empty()) {

            std::set<int> docs_minus_set;             // тут будут id док-тов с минус-словами
            for(const auto& m : query.minus_words) {
                try {
                    const auto& buffer = documents_.at(m);
                    docs_minus_set.insert(buffer.cbegin(),
                                          buffer.cend());    // собираем id
                } catch (std::out_of_range& e) {}
            }

        /* Вычистим из итогового словаря, по которому будем считать IDF нужных нам слов*/
            std::set<int> new_set;
            for(auto& [word,id_set] : docs_map) {
                std::set_difference(id_set.begin(),id_set.end(),
                                    docs_minus_set.begin(),docs_minus_set.end(),
                                    std::inserter(new_set,new_set.end()));
                id_set = new_set;
                new_set.clear();
            }
        } //if
        documents_map = docs_map;                   // помещаем результаты в какой-то "кэш"
        return ;                                    // в роли кэша - глобальная переменная
    }                                               // словаря-карты документа: documents_map

    std::map<std::string,double> Calculate_IDF(const Query& query) const {
        std::map<std::string,double> idf_map;
        SetDocsMap(query);                          // map<string, set<int>>
                                                    // documents_map нужeн для расчёта IDF
        for(const auto& [word,id_set] : documents_map) {
            idf_map[word] = (id_set.size() == static_cast<size_t>(document_count_))
                          ? 0.0
                          : log(static_cast<double>(document_count_) / id_set.size());
        }
        return idf_map;     // теперь у нас есть коллекция word->IDF
    }

    // получаем все релевантные ID документов
    static std::set<int> Flatten() {
        std::set<int> united_ids;
        for(const auto& [word,ids_set] : documents_map)
            united_ids.insert(ids_set.begin(),ids_set.end());

        return united_ids;
    }

    /*std::map<std::string,
             std::map<int,double>> word_to_document_freqs_*/
    std::vector<Document> FindAllDocuments(const Query& query) const {
        auto idf_map = Calculate_IDF(query);          // заполняем word->IDF
        std::vector<Document> matched_documents;      // id, relevance
        std::map<int,double> buffer;                 // аккумулятор сумм произведений IDF*TF по каждому ID
        const std::set<int> all_ids_set=Flatten();   // релевантные ID документов, расчитываем из данных
                                                     // documents_map (внешнего словаря)


        for(const auto& [word,IDF] : idf_map) {
            for(const auto& [id, TF] : word_to_document_freqs_.at(word)) {
                if(all_ids_set.count(id)) {
                    buffer[id] += IDF * TF;
                }
            }
        }

        for(const auto& [doc_id,relevance] : buffer)
            matched_documents.push_back({doc_id,relevance});

        return matched_documents;
    }

public:
    void SetStopWords(const std::string& text) {
        for (const std::string& word : SplitIntoWords(text))
            stop_words_.insert(word);
    }

    /*map<string, map<int, double>> */
    void AddDocument(int document_id,const std::string& document) {
        std::vector<std::string> doc_vector = SplitIntoWordsNoStop(document);
        std::map<std::string,double> word_tf;
        for(const auto& w: doc_vector) {
            word_to_document_freqs_[w][document_id] += 1.0 / doc_vector.size();
            documents_[w].insert(document_id);              // create INDEX
        }
        ++document_count_;
    }

    std::vector<Document> FindTopDocuments(const std::string& raw_query) const {
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query);

        std::sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });

        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT)
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);

        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();
    const std::string query = ReadLine();

    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query))
        std::cout << "{ document_id = "s << document_id << ", "s
             << "relevance = "s << relevance << " }\n"s;

    return 0;
}
