#include "cluster.h"

#include <boost/range/algorithm/nth_element.hpp>

#include <cassert>
#include <set>
#include <vector>

void TNewsCluster::AddDocument(const TDocument& document) {
    Documents.push_back(std::cref(document));
}

uint64_t TNewsCluster::GetTimestamp(float percentile) const {
    assert(!Documents.empty());
    std::vector<uint64_t> clusterTimestamps;
    for (const TDocument& doc : Documents) {
        clusterTimestamps.push_back(doc.FetchTime);
    }
    size_t index = static_cast<size_t>(std::floor(percentile * (clusterTimestamps.size() - 1)));
    boost::range::nth_element(clusterTimestamps, clusterTimestamps.begin() + index);
    return clusterTimestamps[index];
}

uint64_t TNewsCluster::GetFreshestTimestamp() const {
    return std::max_element(Documents.begin(), Documents.end(), [](const TDocument& doc1, const TDocument& doc2) {
        return doc1.FetchTime < doc2.FetchTime;
    })->get().FetchTime;
}

ENewsCategory TNewsCluster::GetCategory() const {
    std::vector<size_t> categoryCount(NC_COUNT);
    for (const TDocument& doc : Documents) {
        ENewsCategory docCategory = doc.Category;
        assert(docCategory != NC_UNDEFINED && docCategory != NC_NOT_NEWS);
        categoryCount[static_cast<size_t>(docCategory)] += 1;
    }
    auto it = std::max_element(categoryCount.begin(), categoryCount.end());
    return static_cast<ENewsCategory>(std::distance(categoryCount.begin(), it));
}
