#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std::literals;
const int MAX_RESULT_DOCUMENT_COUNT = 5;

std::string ReadLine() {
    std::string s;
    std::getline(std::cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
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
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id=0;
    double relevance=0.0;
    int rating=0;
};

template<typename T>
std::vector<T> Split(const std::string& line) {
    std::istringstream is(line);
    return std::vector<T>(std::istream_iterator<T>(is),
                          std::istream_iterator<T>());
}

std::vector<int> ParseStrToIntVector(const std::string& str) {
    std::vector<int> raw_result;

    if(!str.empty()) {
        raw_result = Split<int>(str.c_str());

        if(raw_result.size() == 1 || raw_result[0] == 0) {
            return std::vector<int>();
        } else {
            size_t len = static_cast<size_t>(raw_result[0]);
            raw_result.erase(raw_result.begin());

            if(raw_result.size() > len)
                raw_result.resize(len);
        }
    }
    return raw_result;
}

class SearchServer {
private:
    int document_count_ = 0;
    std::set<std::string> stop_words_;
    std::map<std::string, std::map<int, double>> word_to_document_freqs_;
    std::map<int,int> document_ratings_;


    bool IsStopWord(const std::string& word) const {
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

    struct QueryWord {
        std::string data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(std::string text) const {
        bool is_minus = false;
        // Word shouldn't be empty
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        return {text, is_minus, IsStopWord(text)};
    }

    struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

    Query ParseQuery(const std::string& text) const {
        Query query;
        for (const std::string& word : SplitIntoWords(text)) {
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                } else {
                    query.plus_words.insert(query_word.data);
                }
            }
        }
        return query;
    }

    // Existence required
    double ComputeWordInverseDocumentFreq(const std::string& word) const {
        return log(document_count_ * 1.0 / word_to_document_freqs_.at(word).size());
    }

    std::vector<Document> FindAllDocuments(const Query& query) const {
        std::map<int, double> document_to_relevance;
        for (const std::string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto &[document_id, term_freq] : word_to_document_freqs_.at(word)) {
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        }

        for (const std::string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto &[document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        std::vector<Document> matched_documents;
        for (const auto &[document_id, relevance] : document_to_relevance) {
            matched_documents.push_back({document_id, relevance, document_ratings_.at(document_id)});
        }
        return matched_documents;
    }

    static int ComputeAverageRating(const std::vector<int>& ratings) {
        return ratings.empty()  ? 0
                                : ( ratings.size() == 1 ? ratings[0]
                                                        : std::accumulate(ratings.begin(), ratings.end(),0) / static_cast<int>(ratings.size()) );
    }
//------------
public:
    void SetStopWords(const std::string& text) {
        for (const std::string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const std::string& document,const std::vector<int>& doc_ratings) {
        ++document_count_;
        const std::vector<std::string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const std::string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        document_ratings_[document_id] = doc_ratings.empty() ? 0 : ComputeAverageRating(doc_ratings);
    }

    std::vector<Document> FindTopDocuments(const std::string& raw_query) const {
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query);

        std::sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        const std::string doc_string = ReadLine();
        const std::vector<int> rating = ParseStrToIntVector(ReadLine());

        search_server.AddDocument(document_id, doc_string,rating);
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const std::string query = ReadLine();
    for (auto [document_id, relevance, rating] : search_server.FindTopDocuments(query)) {
        std::cout << "{ document_id = "s << document_id
             << ", relevance = "s << relevance << ", rating = "s << rating <<" }\n"s;
    }
}
