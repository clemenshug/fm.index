#include <Rcpp.h>
#include <string>
#include <vector>

#include <sdsl/suffix_arrays.hpp>

#include "utils.hpp"

using namespace Rcpp;

class FMIndex {
public:
  FMIndex(tree_t index, std::vector<int> boundaries) : index(index), boundaries(boundaries) {};
  FMIndex(CharacterVector text);
  DataFrame find(CharacterVector patterns);
private:
  tree_t index;
  std::vector<int> boundaries;
};

FMIndex::FMIndex(CharacterVector text) {
  boundaries.reserve(text.size());
  int string_length = 0;
  for (auto& x: text) {
    string_length += x.size();
    boundaries.push_back(string_length);
  }
  std::string text_concat;
  text_concat.reserve(string_length);
  for (auto& x: text) {
    text_concat.append(x);
  }
  sdsl::construct_im(index, text_concat, 1);
}

DataFrame FMIndex::find(CharacterVector patterns) {
  std::vector< std::vector<int> > all_locations;
  for (auto& pattern: patterns) {
    auto locations_ = locate(index, pattern.begin(), pattern.end());
    auto n = locations_.size();
    std::vector<int> locations;
    locations.reserve(n);
    for (auto& x: locations_) {
      locations.push_back(x);
    }
    all_locations.push_back(locations);
  }
  int n_total = 0;
  for (auto& locations: all_locations)
    n_total += locations.size();
  IntegerVector pattern_indices(n_total);
  IntegerVector library_indices(n_total);
  IntegerVector positions(n_total);
  int i_total = 0;
  for (int pattern_idx = 0; pattern_idx < patterns.size(); pattern_idx++) {
    auto locations = all_locations[pattern_idx];
    for (auto& location: locations) {
      auto library_index = std::distance(
        boundaries.begin(),
        std::upper_bound(boundaries.begin(), boundaries.end(), location)
      );
      auto position = location;
      if (library_index > 0)
        position -= boundaries[library_index - 1];
      pattern_indices[i_total] = pattern_idx + 1;
      library_indices[i_total] = library_index + 1;
      positions[i_total] = position + 1;
      i_total++;
    }
  }
  return DataFrame::create(
    Named("pattern_index") = pattern_indices,
    Named("library_index") = library_indices,
    Named("position") = positions
  );
}

//' Construct FM Index
//'
//' @param strings Vector of strings to construct FM index from
//' @export
// [[Rcpp::export]]
SEXP construct_fm_index(CharacterVector strings) {
  auto* fm_index = new FMIndex(strings);
  XPtr<FMIndex> ptr(fm_index);
  return ptr;
}

//' Find query in FM Index
//'
//' @param query Strings to find in FM index
//' @param index Pointer to index created with [construct_fm_index()]
//' @export
// [[Rcpp::export]]
DataFrame fm_index_find(CharacterVector query, SEXP index) {
  Rcpp::XPtr<FMIndex> index_ptr(index);
  return index_ptr->find(query);
}
