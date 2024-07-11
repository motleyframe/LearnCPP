#include <algorithm>
#include <iostream>
#include <vector>

using namespace std::string_literals;

struct Document {
    int id=0;
    int rating=0;
};

std::ostream& operator<<(std::ostream& os, const Document& doc) {
    os  << "{ id = "s << doc.id
        << ", rating = "s
        << doc.rating << " } \n"s;
    return os;
}

void PrintDocuments(std::vector<Document> documents, size_t skip_start, size_t skip_finish) {
    // отсортировать документы и вывести не все
    if(documents.empty() || skip_start+skip_finish >= documents.size()) {
        return ;
    }

    std::sort(documents.begin(),documents.end(),[]
            ( const auto& r,const auto& l ) {
                return r.rating > l.rating;
        } );

    std::vector<Document> new_doc;

    for(size_t i=skip_start; i+skip_finish < documents.size() ; ++i)
        new_doc.push_back(documents[i]);

    documents=new_doc;

    for(const auto& d : documents)
        std::cout<<d;

    return ;
}

int main() {
    PrintDocuments({{100, 5}, {101, 7}, {102, -4}, {103, 9}, {104, 1}}, 1, 2);
}
