#include <iostream>
#include <set>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

std::string ReadLine() {
    std::string s;
    std::getline(std::cin, s);
    return s;
}

int ReadLineWithNumber() {
    int n = 0;
    std::cin >> n;
    ReadLine();
    return n;
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

std::set<std::string> ParseStopWords(const std::string& text) {
    std::set<std::string> stop_words;
    for (const std::string& word : SplitIntoWords(text)) {
        stop_words.insert(word);
    }
    return stop_words;
}

std::vector<std::string> SplitIntoWordsNoStop(const std::string& text, const std::set<std::string>& stop_words) {
    std::vector<std::string> words;
    for (const std::string& word : SplitIntoWords(text)) {
        if (!stop_words.count(word))
            words.push_back(word);
    }
    return words;
}

void AddDocument(std::vector<std::pair<int,std::vector<std::string>>>& Docs,
                 const std::set<std::string>& stop_words,
                 int& doc_id,
                 const std::string& document) {
    const std::vector<std::string> words = SplitIntoWordsNoStop(document, stop_words);
    Docs.push_back( {doc_id,words} );
}

std::set<std::string> ParseQuery(const std::string& text,
                                 const std::set<std::string>& stop_words) {
    set<string> query_words;
    for (const std::string& word : SplitIntoWordsNoStop(text, stop_words)) {
        query_words.insert(word);
    }
    return query_words;
}

int MatchDocument(const std::pair<int,std::vector<std::string>>& document_words,
                  const std::set<std::string>& query_words) {
    int rel=0;
    std::vector<std::string> buf = document_words.second;

    for(const auto& b : buf) {
        if(query_words.count(b))
            ++rel;
    }

    return rel;
}

// Для каждого найденного документа возвращает его {rel,id}
std::vector<std::pair<int,int>> FindDocuments(const std::vector<std::pair<int,vector<string>>>& Docs,
                                              const std::set<std::string>& stop_words,
                                              const std::string& query) {
    const std::set<std::string> query_words = ParseQuery(query, stop_words);

    std::vector<std::pair<int,int>> matched_documents;
    int rel=0;

    for (const auto& d : Docs) {
        rel=MatchDocument(d, query_words);
        if(rel) {
            matched_documents.push_back({d.first,rel});
        }
    }
    return matched_documents;
}

int main() {
    const std::string stop_words_joined = ReadLine();
    const std::set<std::string> stop_words = ParseStopWords(stop_words_joined);

    // Read documents
    std::vector<std::pair<int,vector<string>>> DB;
    const int N = ReadLineWithNumber();
    for (int doc_id = 0; doc_id < N; ++doc_id) {
        AddDocument(DB, stop_words, doc_id, ReadLine());
    }

    const std::string query = ReadLine();

    for (auto& docs_found : FindDocuments(DB, stop_words, query))
        std::cout << "{ document_id = "s << docs_found.first
                  << ", relevance = "s << docs_found.second <<" }\n"s ;

    return EXIT_SUCCESS;
}
