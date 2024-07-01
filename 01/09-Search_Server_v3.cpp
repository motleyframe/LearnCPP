#include <algorithm>
#include <iostream>
#include <set>
#include <chrono>
#include <string>
#include <vector>
#include <numeric>

using namespace std::string_literals;

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
    int id;
    int relevance;
};

class SearchServer {
    private:
    struct DocumentContent {
        int id = 0;
        std::vector<std::string> words;
    };

    struct QueryWord {
        std::string data;
        bool is_minus;
        bool is_stop;
    };

    struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

    std::vector<DocumentContent> documents_;

    std::set<std::string> stop_words_;

    inline bool IsStopWord(const std::string& word) const {
        return stop_words_.count(word) > 0;
    }

    std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const {
        std::vector<std::string> words;
        for (const std::string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
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

    std::vector<Document> FindAllDocuments(const Query& query) const {
        std::vector<Document> matched_documents;
        for (const auto& document : documents_) {
            const int relevance = MatchDocument(document, query);
            if (relevance > 0) {
                matched_documents.push_back({document.id, relevance});
            }
        }
        return matched_documents;
    }

    static int MatchDocument(const DocumentContent& content, const Query& query) {
        if (query.plus_words.empty())
            return 0;

        auto& pWords = content.words;
        auto& pMinus = query.minus_words;
        auto& pPlus = query.plus_words;



        //  at least 1 minus_word in the query => skip this document;
       if(std::find_first_of(pWords.cbegin(),pWords.cend(),
                              pMinus.cbegin(),pMinus.cend()) != pWords.cend())
            return 0;


        return std::accumulate (pPlus.begin(),pPlus.end(),0,[pWords]
                               (int i, const std::string& q) {
                                   return i + (std::count(pWords.begin(),pWords.end(),q) ? 1 : 0);
                               } );
    }

public:
    void SetStopWords(const std::string& text) {
        for (const std::string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const std::string& document) {
        const std::vector<std::string> words = SplitIntoWordsNoStop(document);
        documents_.push_back({document_id, words});
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

    auto begin = std::chrono::steady_clock::now();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query))
        std::cout << "{ document_id = "s << document_id << ", "s
             << "relevance = "s << relevance << " }\n"s;
    auto end = std::chrono::steady_clock::now();
    std::cout<<"-----------------\n"<<std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count()<<" microseconds\n"s;

    return 0;
}
