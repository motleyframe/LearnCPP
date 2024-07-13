#include <algorithm>
#include <iostream>
#include <vector>

using namespace std::string_literals;

struct Document {
    int id=0;
    double relevance=0.0;
    int rating=0.0;
};

void SortDocuments(std::vector<Document>& matched_documents) {
    std::sort(matched_documents.begin(), matched_documents.end(),
         [](const Document& lhs, const Document& rhs) {
             return lhs.rating > rhs.rating ||
                    (lhs.rating == rhs.rating && lhs.relevance > rhs.relevance);
         } );
}

int main() {
    std::vector<Document> documents = {{100, 0.5, 4}, {101, 1.2, 4}, {102, 0.3, 5}};
    SortDocuments(documents);
    for (const Document& document : documents) {
        std::cout << document.id << ' ' << document.relevance << ' ' << document.rating << std::endl;
    }

    return 0;
}
