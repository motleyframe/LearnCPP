#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

using namespace std::string_literals;
const int MAX_RESULT_DOCUMENT_COUNT = 5;

struct Document {
    int id;
    int relevance;
};

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
            } else word += c;
        }
        if (!word.empty())
            words.push_back(word);

        return words;
    }

class SearchServer {
private:
    struct DocumentContent {
        int id=0;
        std::vector<std::string> words;
    };

    std::vector<DocumentContent> documents_;
    std::set<std::string> stop_words_;

    static int MatchDocument(const DocumentContent& content, const std::set<std::string>& query_words) {
        if (query_words.empty())
            return 0;

        std::set<std::string> matched_words;

        for (const std::string& word : content.words) {
            if (matched_words.count(word))
                continue;

            if ( query_words.count(word) ) {
                matched_words.insert(word);
            }
        }
        return static_cast<int>(matched_words.size());
    }

    std::vector<Document> FindAllDocuments(const std::set<std::string>& query_words) const {
        std::vector<Document> matched_documents;
        for (const auto& d : documents_) {
            const int relevance = MatchDocument(d, query_words);
            if (relevance)
                matched_documents.push_back({d.id,relevance});
        }
        return matched_documents;
    }

    std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const {
        std::vector<std::string> words;
        for (const std::string& word : SplitIntoWords(text)) {
            if( !stop_words_.count(word) ) {
                words.push_back(word);
            }
        }
        return words;
    }

    std::set<std::string> ParseQuery(const std::string& text) const {
        std::set<std::string> query_words;
        for (const std::string& w : SplitIntoWordsNoStop(text))
            query_words.insert(w);

        return query_words;
    }

public:
    void SetStopWords(const std::string& text) {
        std::set<std::string> stop_words;
        for (const std::string& word : SplitIntoWords(text))
            stop_words_.insert(word);
    }

    void AddDocument(int document_id, const std::string& document) {
        const std::vector<std::string> words = SplitIntoWordsNoStop(document);
        documents_.push_back({document_id,words});
    }

    std::vector<Document> FindTopDocuments(const std::string& raw_query) const {
        const std::set<std::string> query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);
        std::sort(matched_documents.begin(), matched_documents.end(),[]( const auto& left,const auto& right )
                                                                        { return left.relevance > right.relevance; }
                                                                    );
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT)
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);

        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer s;
    s.SetStopWords(ReadLine());

    const int doc_count = ReadLineWithNumber();
    for (int doc_id = 0; doc_id < doc_count; ++doc_id) {
        s.AddDocument(doc_id, ReadLine());
    }
    return s;
}

int main() {
    const SearchServer server = CreateSearchServer();
    const std::string raw_query = ReadLine();

    for(auto [id,relevance] : server.FindTopDocuments(raw_query))
        std::cout << "{ document_id = "s
                  << id << ", relevance = "s
                  << relevance << " }\n"s;
    return 0;
}
