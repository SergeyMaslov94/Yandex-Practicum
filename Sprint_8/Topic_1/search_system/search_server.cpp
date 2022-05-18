#include "search_server.h"
#include <stdexcept>
#include <numeric>
//========================================================================================
// public
SearchServer::SearchServer()
= default;

SearchServer::SearchServer(const std::string &text) {
    for (const std::string& word : SplitIntoWords(text)) {
        if(!IsValidWord(word))
            throw std::invalid_argument("В стоп-словах содержатся недопустимые символы!");

        stop_words_.insert(word);
    }
}

void SearchServer::AddDocument(
        int document_id,
        const std::string& document,
        DocumentStatus status,
        const std::vector<int>& ratings) {

    // Проверка на отрицательный ID
    if(document_id < 0) {
        throw std::invalid_argument("документ содержит отрицательный id!");
    }
    // Проверка на наличие документов с одинаковым ID
    if(documents_.count(document_id)) {
        throw std::invalid_argument("документ с таким id уже существует!");
    }

    const std::vector<std::string> words = SplitIntoWordsNoStop(document);
    const double inv_word_count = 1.0 / words.size();

    for (const std::string& word : words) {
        // Проверка на наличие спец символов в документе
        if(!IsValidWord(word)) {
            throw std::invalid_argument("документ содержит недопустимые символы!");
        } else {
            word_to_document_freqs_[word][document_id] += inv_word_count;
            ids_and_words_with_freqs[document_id][word] += inv_word_count;
        }
    }

    documents_.emplace(document_id,DocumentData{ComputeAverageRating(ratings), status});
    document_ids_.insert(document_ids_.begin(), document_id);
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, DocumentStatus status) const {
    return FindTopDocuments(raw_query,[status]([[maybe_unused]] int document_id, DocumentStatus document_status, [[maybe_unused]] int rating){return ((document_status == status));});
}

int SearchServer::GetDocumentCount() const {
    return documents_.size();
}

const std::map<std::string, double>& SearchServer::GetWordFrequencies(int document_id) const{
    static std::map<std::string, double> empty_result;
    if(ids_and_words_with_freqs.count(document_id) == 0)
        return empty_result;

    return ids_and_words_with_freqs.at(document_id);
}

void SearchServer::RemoveDocument(int document_id) {

    if(documents_.count(document_id) == 0)
        return;

    documents_.erase(document_id);
    document_ids_.erase(document_id);

    for(const auto word : GetWordFrequencies(document_id)) {
        word_to_document_freqs_.at(word.first).erase(document_id);
    }

    ids_and_words_with_freqs.erase(document_id);
}

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(
        const std::string& raw_query,
        int document_id) const {

    const Query query = ParseQuery(raw_query);
    std::vector<std::string> matched_words;

    for (const std::string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }

        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }

    for (const std::string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }

        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }
    return { matched_words, documents_.at(document_id).status };
}

//========================================================================================
// private
bool SearchServer::NegativeWordErr(const std::string& word) const {
    // если после символа 'минус' отсутствует текст
    if(word.size() == 1) {
        if(word[0] == '-') {
            return false;
        }
    }
    // если в запросе более одного минуса перед минус-словами
    if(word.size() > 1) {
        if (word[0] == '-' && word[1] == '-') {
            return false;
        }
    }
    return true;
}

bool SearchServer::IsValidWord(const std::string& word) {
    // A valid word must not contain special characters
    return none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
    });
}

bool SearchServer::IsStopWord(const std::string& word) const {
    return stop_words_.count(word) > 0;
}

std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string& text) const {
    std::vector<std::string> words;
    for (const std::string& word : SplitIntoWords(text)) {
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

int SearchServer::ComputeAverageRating(const std::vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }

    int rating_sum = std::accumulate(ratings.begin(), ratings.end(), 0);
    return rating_sum / static_cast<int>(ratings.size());
}

SearchServer::QueryWord SearchServer::ParseQueryWord(std::string text) const {
    bool is_minus = false;
    if (text[0] == '-') {
        is_minus = true;
        text = text.substr(1);
    }
    return {text, is_minus,IsStopWord(text)};
}

SearchServer::Query_parall SearchServer::ParseQueryWordParall(std::vector<std::string> texts) {
    Query_parall query;
    for (auto text : texts) {
        
        if (duplicate_words_in_parse_query_.count(text) != 0) {
            continue;
        }
        duplicate_words_in_parse_query_.insert(text);

        if (!IsStopWord(text)) {
            if (text[0] == '-') {
                query.minus_words.push_back(text.substr(1));
            }
            else {
                query.plus_words.push_back(text);
            }
        }
    }

    return query;
}

SearchServer::Query SearchServer::ParseQuery(const std::string& text) const {
    Query query;
    for (const std::string& word : SplitIntoWords(text)) {
        // некорректный ввод минус слов
        if(!NegativeWordErr(word)) {
            throw std::invalid_argument("некорректный ввод минус слов!");
        }
        // Проверка на наличие спец символов в запросе
        if(!IsValidWord(word)) {
            throw std::invalid_argument("документ содержит недопустимые символы!");
        }
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

double SearchServer::ComputeWordInverseDocumentFreq(const std::string& word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}
