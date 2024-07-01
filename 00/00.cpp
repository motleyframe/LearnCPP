#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <string>

using namespace std;
typedef std::vector<std::vector<std::string>> Docs;

std::string read_line() {
    std::string line;
    std::getline(std::cin,line);
    return line;
}

int read_number() {
    int n=0;
    std::cin>>n;
    read_line();
    return n;
}

std::vector<std::string> SplitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::string str;

    for(char c : text) {
        if(c==' ') {
            if(!str.empty()) {
                words.push_back(str);
                str.clear();
            }
        } else str+=c;
    }
    if(!str.empty())
        words.push_back(str);

    return words;
}

std::vector<std::string> SplitIntoWordsNoStop(const string& text, const std::set<std::string>& stop_words) {
    std::vector<std::string> words;

    for(const string& w : SplitIntoWords(text))
        if(!stop_words.count(w))
            words.push_back(w);
    return words;
}

std::set<std::string> ParseQuery(const std::string& text,const std::set<std::string>& stop_words) {
    const std::vector<std::string> filtered_query = SplitIntoWordsNoStop(text,stop_words);
    const std::set<std::string> query_words(filtered_query.begin(),filtered_query.end());
    return query_words;
}

bool MatchDocument(const std::vector<std::string>& doc, const std::set<std::string>& search_query) {
    for(const auto& w : doc)
        if(search_query.count(w))
            return true;
    return false;
}

std::vector<int> FindDocuments(const Docs& DB, const std::set<std::string>& stop_words,const std::string& raw_search_query) {
    std::vector<int> matched_document;
    const std::set<std::string> search_query = ParseQuery(raw_search_query,stop_words);

    for(size_t i=0; i<DB.size();++i) {
        if(MatchDocument(DB[i],search_query))
            matched_document.push_back(i);
    }

    return matched_document;
}

void AddDocument(Docs& DB, const std::set<std::string>& stop_words,const std::string& text) {
    DB.push_back(SplitIntoWordsNoStop(text,stop_words));
}


int main() {
    Docs DB;
    const std::string stop_words_raw_string = read_line();
    const std::vector<std::string> stop_words_raw_vector = SplitIntoWords(stop_words_raw_string);
    const std::set<std::string> stop_words(stop_words_raw_vector.begin(), stop_words_raw_vector.end());
    const int doc_counter = read_number();

    for(int i=0;i<doc_counter;++i)
        AddDocument(DB,stop_words,read_line());

    const std::string raw_search_query = read_line();

    for(const int& i : FindDocuments(DB,stop_words,raw_search_query))
        std::cout<<"{ document_id = "s << i << " }\n"s;

    return 0;
}
