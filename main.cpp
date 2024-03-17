#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
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
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }
    
    void AddDocumentCount(int n){
        document_count_ = n;
    }

    void AddDocument(int document_id, const string& document) {

        const vector<string> words = SplitIntoWordsNoStop(document);
               
        for (const string & word : words){
            if ((!documents_.count(word)) || (!documents_[word].count(document_id))){

                double tf = count_if(words.begin(), words.end(), 
                    [&word](const string & word_){
                        return word == word_;
                    });
                    
                documents_[word][document_id] = tf/words.size();
            }
        }
        

    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:

    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };
    

    map<string, map<int, double>> documents_;
    set<string> stop_words_;
    int document_count_ = 0;


    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    Query ParseQuery(const string& text) const {
        Query query;
        for (const string & word : SplitIntoWordsNoStop(text)) {
            if (word[0] == '-'){
                string word_temp;
                for (auto it = ++word.begin();it!=word.end();++it){
                    word_temp += *it;
                }
                query.minus_words.insert(word_temp);
            }
            else{
                query.plus_words.insert(word);
            }
        }
        return query;
    }

    vector<Document> FindAllDocuments(const Query & query) const {
        map<int, double> ids_relevances;
        vector<Document> matched_documents;
        
        for (const string & word : query.plus_words){
            if (documents_.count(word)){
                for (auto id_tf : documents_.at(word)){
                    ids_relevances[id_tf.first] += id_tf.second*
                        log(static_cast<double>(document_count_)/documents_.at(word).size());
                }
            }
        }
                     
        for (const string & word : query.minus_words){
            if (documents_.count(word)){
                for (const auto &  id_relevance : documents_.at(word)){
                    ids_relevances.erase(id_relevance.first);
                }
            }
        }
               
        
        
        for (auto & id_relevance : ids_relevances){
            matched_documents.push_back({id_relevance.first, id_relevance.second});
        }
        
        return matched_documents;
        
    }
};


SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    search_server.AddDocumentCount(document_count);
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& doc : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << doc.id << ", "
             << "relevance = "s << doc.relevance << " }"s << endl;
    }
    return 0;
}















