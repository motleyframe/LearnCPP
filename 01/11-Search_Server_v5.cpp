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
std::set<int> all_ids;              // тут будут id только тех документов, которые
                                    // содержат плюс-слова, но не содержат минус-слов
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

    /*Получить set, в котором будут id документов,
     *в которых есть плюс-слова, но притом нету минус-слов*/
    void GetDocIDs(const Query& query,std::set<int>& all_ids_set) const {
        for(const auto& p : query.plus_words) {
            try {                                     //try-catch чтобы отлавливать исключение
                const auto& t = documents_.at(p);   //вследствие обращения по несуществующему ключу
                all_ids_set.insert(t.cbegin(),     // тут будут id док-тов с плюс-словами
                                   t.cend());
            } catch (std::out_of_range& e) { }
        }

        for(const auto& x : all_ids_set)
            std::cout<<x<<' ';
        std::cout<<std::endl;
        /* Пробегаемся по минус-словам, проверяем, есть ли каждое из этих слов в индексе */
        /* Вычистим из итогового set-a те id док-тов, где есть минус-слова*/
        std::set<int> minus_set;              // тут будут id док-тов с минус-словами
        for(const auto& m : query.minus_words) {
            try {
                const auto& t = documents_.at(m);
                minus_set.insert(t.begin(),t.end());
            } catch (std::out_of_range& e) { }
        }

        std::set<int> diff;
        std::set_difference(all_ids_set.begin(),all_ids_set.end(),
                            minus_set.begin(),minus_set.end(),
                            std::inserter(diff,diff.end()));

        if(!diff.empty())
            all_ids_set = diff;                      // запишем рез-т в переменную

        for(const auto& x : all_ids_set)
            std::cout<<x<<' ';
        std::cout<<std::endl;
        return ;                                    // класса
    }

    std::map<std::string,double> Calculate_IDF(const Query& query,std::set<int>& all_ids_set) const {
        std::map<std::string,double> idf_map;
        GetDocIDs(query,all_ids_set);               // для расчёта IDF необходимо
                                                    // рассчитать множество с только нужными ID док-тов
        try {                                       // (без минус-слов)
            for(const auto& word : query.plus_words) {
                const auto& t = documents_.at(word);
                std::set<int> filtered_ids;         // тут будут только те id из
                                                    //all_ids_, где есть слово word
                std::set_intersection(all_ids_set.cbegin(),all_ids_set.cend(),
                                      t.cbegin(),t.cend(),      //пересечение множеств
                                      std::inserter(filtered_ids,filtered_ids.end()));
                idf_map[word] = filtered_ids.size() == static_cast<size_t>(document_count_)
                              ? 0.0                 // если слово есть в каждом документе
                              : ( log(static_cast<double>(document_count_)) / filtered_ids.size() );
            }
        } catch(std::out_of_range& e) { }
        return idf_map;     // теперь у нас есть коллекция word->IDF
    }

    /*std::map<std::string,
             std::map<int,double>> word_to_document_freqs_*/
    std::vector<Document> FindAllDocuments(const Query& query,std::set<int>& all_ids_set) const {
        auto idf_map = Calculate_IDF(query,all_ids_set);          // заполняем word->IDF
        std::vector<Document> matched_documents;      // id, relevance
        std::map<int,double> relevance;               // IDF*TF по каждому id

        for(const auto& [word,IDF] : idf_map) {
            for(const auto& [id, TF] : word_to_document_freqs_.at(word)) {
                if(all_ids_set.count(id)) {              // только те документы, где есть слово word
                    relevance[id] += IDF * TF;
                }
            }
        }

        for(const auto& [doc_id,rel] : relevance)
            matched_documents.push_back({doc_id,rel});

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
        for(const auto& w : doc_vector) {
            word_tf[w] += 1.0 / doc_vector.size();
        }

        for(const auto& [word,tf] : word_tf) {
            word_to_document_freqs_[word].insert({document_id,tf});
            documents_[word].insert(document_id);              // create INDEX
        }
        ++document_count_;
    }

    std::vector<Document> FindTopDocuments(const std::string& raw_query) const {
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query,all_ids);

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
