#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>
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
    int relevance=0;
};

class SearchServer {
private:
    std::map<std::string,std::set<int>> documents_;         // search index
    std::set<std::string> stop_words_;

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

    std::map<int,int> FindRelevance(const Query& query) const {
        std::map<int,int> relevance_map;

        for(const auto& p : query.plus_words) {
            try {
                for(const auto& id : documents_.at(p))
                    ++relevance_map[id];

            } catch (std::out_of_range& e) {}

        }

       for(const auto& m : query.minus_words) {
             try {
                for(const auto& id : documents_.at(m)) {
                    try {
                        relevance_map.erase(id);
                    } catch (std::out_of_range& e) {}
                }
             } catch (std::out_of_range& e) {}
         }
         return relevance_map;
    }

    std::vector<Document> FindAllDocuments(const Query& query) const {
        std::vector<Document> matched_documents;
        std::map<int,int> relevance_map=FindRelevance(query);

        for(const auto& [id,rel] : relevance_map)
            matched_documents.push_back({id,rel});

        return matched_documents;
    }

public:
    void SetStopWords(const std::string& text) {
        for (const std::string& word : SplitIntoWords(text))
            stop_words_.insert(word);
    }

    void AddDocument(int document_id, const std::string& document) {                    // REDO
        for(const auto& w: SplitIntoWordsNoStop(document))
            documents_[w].insert(document_id);
    }

    std::vector<Document> FindTopDocuments(const std::string& raw_query) const {        // REDO ??
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
