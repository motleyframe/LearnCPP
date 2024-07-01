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

        for(const auto& m : query.minus_words) {
            if(query.plus_words.count(m)) {
                query.plus_words.erase(m);
                std::cout<<"AAA";
            }
        }

        return query;
    }

    std::set<int> GetDocsID(const Query& query) const {
        std::set<int> valid_ids;


        std::cout<<"PLUS: \n";
        for(const auto& s : query.plus_words) {
            std::cout<<s<<' ';
        }
        std::cout<<std::endl;

        std::cout<<"MINUS: \n";
        for(const auto& s : query.minus_words) {
            std::cout<<s<<' ';
        }
        std::cout<<std::endl;

        for(const auto& p : query.plus_words) {
            try {
                const auto& plus = documents_.at(p);
                valid_ids.insert(plus.cbegin(),plus.cend());
            } catch(std::out_of_range& e) { std::cerr << "query.plus_words: "s <<e.what() << std::endl; continue; }
        }

        if(!query.minus_words.empty()) {
            std::set<int> minus_words_set;
            for(const auto& m : query.minus_words) {
                try {
                    std::cout<<m<<' '<<std::endl;
                    const auto& minus = documents_.at(m);
                    minus_words_set.insert(minus.begin(),minus.end());
                } catch(std::out_of_range& e) { std::cerr << "query.minus_words: "s << e.what() << std::endl; continue; }
            }

            std::set<int> diff;
            std::set_difference(valid_ids.begin(),valid_ids.end(),
                                minus_words_set.begin(),valid_ids.end(),
                                std::inserter(diff,diff.end()));
            valid_ids=diff;
        }
        return valid_ids;
    }

    std::pair<std::map<std::string,double>,
              std::set<int>> Calculate_IDF(const Query& query) const {
        const std::set<int> valid_ids = GetDocsID(query);

        std::map<std::string,double> idf_map;

        for(const auto& word : query.plus_words) {
            try {
                const auto& all_ids = documents_.at(word);
                 std::set<int> common;
                 std::set_intersection(valid_ids.cbegin(),valid_ids.cend(),
                                       all_ids.cbegin(),all_ids.cend(),
                                       std::inserter(common,common.end()));
                idf_map[word] = common.size() == static_cast<size_t>(document_count_)
                              ? 0.0                 // если слово есть в каждом документе
                              : log(static_cast<double>(document_count_) / common.size());
                //std::cout<<idf_map[word]<<" : "s<<common.size()<<std::endl;
            } catch(std::out_of_range& e) { std::cerr << "Calculate_IDF: "s << e.what() << std::endl; continue; }
        }
        return {idf_map,valid_ids};     // теперь у нас есть коллекция word->IDF
    }

    /*std::map<std::string,
             std::map<int,double>> word_to_document_freqs_*/
    std::vector<Document> FindAllDocuments(const Query& query) const {
        const auto idf_map = Calculate_IDF(query);          // заполняем word->IDF
        std::vector<Document> matched_documents;      // id, relevance
        std::map<int,double> buffer;                 // аккумулятор сумм произведений IDF*TF по каждому ID

        for(const auto& [word,IDF] : idf_map.first) {
            for(const auto& [id, TF] : word_to_document_freqs_.at(word)) {
                if(idf_map.second.count(id)) {
                    buffer[id] += IDF * TF;
                }
            }
        }

        for(const auto& [doc_id,relevance] : buffer)
            matched_documents.push_back( {doc_id,relevance} );

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
