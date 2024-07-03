#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

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
    for(const char c : text) {
        if(c == ' ') {
            if(!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else word += c;
    }
    if(!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
private:
    struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

    std::map<std::string, std::map<int, double>> word_documents_freq_;
    std::set<std::string> stop_words_;
    int document_count_ = 0;

    bool IsStopWord(const std::string& word) const {
        return stop_words_.count(word);
    }

    std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const {
        std::vector<std::string> words;
        for(const std::string& word : SplitIntoWords(text)) {
            if(!IsStopWord(word))
                words.push_back(word);
        }
        return words;
    }

    Query ParseQuery(const std::string& text) const {
        Query query;
        for(const std::string& w : SplitIntoWordsNoStop(text)) {
            if(w[0] == '-') {
                std::string trimmed = w.substr(1);
                if (!IsStopWord(trimmed))
                    query.minus_words.insert(trimmed);
            } else
                query.plus_words.insert(w);
        }
        return query;
    }

    double CalcWordIDF(const std::string& word) const {
            const auto& freq = word_documents_freq_.at(word);
            return freq.size() == static_cast<size_t>(document_count_) || freq.size() == 0
                       ? 0.0
                       : log(document_count_ / static_cast<double>(freq.size()));
    }

    // Получить ID документов, в которых есть минус-слова
    std::set<int> GetInvalidIDs(const Query& query) const {
        std::set<int> invalid_ids;
        for(const std::string& m : query.minus_words) {
            if(!word_documents_freq_.count(m))
                continue;

            for(const auto &[bad_id, _] : word_documents_freq_.at(m))
                invalid_ids.insert(bad_id);
        }
        return invalid_ids;
    }

    std::vector<Document> FindAllDocuments(const Query& query) const {
        std::map<int, double> tf_idf;
        const std::set<int> invalid_ids = GetInvalidIDs(query);

        for(const auto& p : query.plus_words) {
            if(!word_documents_freq_.count(p))
                continue;

            double idf = CalcWordIDF(p);
            const auto& freq = word_documents_freq_.at(p);

            for(const auto& [id, _] : freq)
                //только документы, где нет минус-слов
                if(!invalid_ids.count(id))
                    tf_idf[id] += idf * freq.at(id);
        }

        std::vector<Document> matched_documents;
        for (const auto& [id, relevance] : tf_idf)
            matched_documents.push_back( {id, relevance} );

        return matched_documents;
    }

public:
    void SetStopWords(const std::string& text) {
        for(const std::string& word : SplitIntoWords(text))
            stop_words_.insert(word);
        return ;
    }

    void AddDocument(int doc_id, const std::string& document) {
        const std::vector<std::string> words = SplitIntoWordsNoStop(document);
        double offset = 1.0 / words.size();

        for(const std::string& w : words) {
            word_documents_freq_[w][doc_id] += offset;
        }
        ++document_count_;
    }

    std::vector<Document> FindTopDocuments(const std::string& raw_query) const {
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query);

        std::sort(matched_documents.begin(), matched_documents.end(),
             [](const auto& lhs, const auto& rhs) {
                 return lhs.relevance > rhs.relevance;
             });

        if(matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT)
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);

        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for(int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }
    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const std::string query = ReadLine();
    for(const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        std::cout << "{ document_id = "s << document_id << ", "s
                  << "relevance = "s << relevance << " }\n"s;
    }
    return 0;
}
