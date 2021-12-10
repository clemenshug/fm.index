#include <string>
#include <vector>
#include <fstream>

#include <sdsl/suffix_arrays.hpp>
#include <sdsl/cereal.hpp>

// Realloc is defined in both the cereal dependency rapidjson and in core R
// Have to include Rcpp after sdsl / cereal
#include <Rcpp.h>

#include <stringi.h>

using namespace Rcpp;

using tree_t = sdsl::csa_wt<>;

class FMIndex {
public:
  FMIndex() {};
  FMIndex(tree_t index, std::vector<int> boundaries) : index(index), boundaries(boundaries) {};
  FMIndex(CharacterVector text);
  DataFrame find(CharacterVector patterns);
  template<class Archive>
  void serialize(Archive& archive) {
    archive(index, boundaries);
  };
  void load_file(String path);
  void save_file(String path);
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

void FMIndex::load_file(String path) {
  std::ifstream in_file(path, std::ios::binary);
  cereal::BinaryInputArchive archive(in_file);
  archive(*this);
}

void FMIndex::save_file(String path) {
  std::ofstream out_file(path, std::ios::binary);
  cereal::BinaryOutputArchive archive(out_file);
  archive(*this);
}

List wrap_index(FMIndex* index) {
  XPtr<FMIndex> index_ptr(index);
  auto wrapped = List::create(
    Named("index") = index_ptr,
    Named("n") = index->boundaries.size(),
    Named("n_bytes") = index->index.size()
  );
  wrapped.attr("class") = "fmindex";
  return wrapped;
}

FMIndex* unwrap_index(List index) {
  if (as<std::string>(index.attr("class")) != "fmindex")
    stop("Not an FMIndex");
  return (FMIndex*) R_ExternalPtrAddr(index["index"]);
}

//' Construct FM Index
//'
//' @param strings Vector of strings to construct FM index from
//' @param case_sensitive Build case-sensitive index if TRUE
//' @export
// [[Rcpp::export]]
List fm_index_construct(CharacterVector strings, bool case_sensitive = false) {
  if (!case_sensitive)
    strings = stri_trans_tolower(strings);
  auto* fm_index = new FMIndex(strings);
  return wrap_index(fm_index);
}

//' Find query in FM Index
//'
//' @param query Strings to find in FM index
//' @param index Pointer to index created with [fm_index_construct()]
//' @export
// [[Rcpp::export]]
DataFrame fm_index_find(CharacterVector query, List index) {
  return unwrap_index(index)->find(query);
}

//' Save FM Index
//'
//' @param index Pointer to index created with [fm_index_construct()]
//' @param path Path to save index to
//' @export
// [[Rcpp::export]]
void fm_index_save(List index, String path) {
  unwrap_index(index)->save_file(path);
}

//' Load FM Index
//'
//' @param path Path to load index from
//' @export
// [[Rcpp::export]]
List fm_index_load(String path) {
  auto* fm_index = new FMIndex();
  fm_index->load_file(path);
  return wrap_index(fm_index);
}
