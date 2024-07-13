#include <algorithm>
#include <iostream>
#include <vector>
#include <tuple>

using namespace std::string_literals;

enum class Status {
    ACTUAL, EXPIRED, DELETED
};

struct Document {
    int id=0;
    Status status = Status::ACTUAL;
    double relevance=0.0;
    int rating=0;
};

/*104 0 0.3 5
102 0 1.2 4
100 0 0.5 4
101 1 0.5 4
103 2 1.2 4
*/

void SortDocuments(std::vector<Document>& matched_documents) {
    std::sort(matched_documents.begin(), matched_documents.end(),
         [](const Document& lhs, const Document& rhs) {
            return lhs.status == rhs.status ? std::pair(lhs.rating,lhs.relevance) > std::pair(rhs.rating,rhs.relevance)
                                          : (lhs.status < rhs.status ? true : false);
        } );
}

int main() {
    std::vector<Document> documents = {
        {100, Status::ACTUAL, 0.5, 4},
        {101, Status::EXPIRED, 0.5, 4},
        {102, Status::ACTUAL, 1.2, 4},
        {103, Status::DELETED, 1.2, 4},
        {104, Status::ACTUAL, 0.3, 5},
    };
    SortDocuments(documents);
    for (const Document& document : documents) {
        std::cout << document.id << ' ' << static_cast<int>(document.status) << ' ' << document.relevance
             << ' ' << document.rating << std::endl;
    }

    return 0;
}
