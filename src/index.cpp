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
  FMIndex(const CharacterVector& text);
  DataFrame find(const CharacterVector& patterns);
  template<class Archive>
  void serialize(Archive& archive) {
    archive(index, boundaries);
  };
  void load_file(const String& path);
  void save_file(const String& path);
  tree_t index;
  std::vector<int> boundaries;
};

FMIndex::FMIndex(const CharacterVector& text) {
  boundaries.reserve(text.size());
  int string_length = 0;
  for (const auto& x: text) {
    string_length += x.size();
    boundaries.push_back(string_length);
  }
  std::string text_concat;
  text_concat.reserve(string_length);
  for (const auto& x: text) {
    text_concat.append(x);
  }
  sdsl::construct_im(index, text_concat, 1);
}

DataFrame FMIndex::find(const CharacterVector& patterns) {
  std::vector< std::vector<int> > all_locations;
  int n_total = 0;
  for (const auto& pattern: patterns) {
    const auto locations_ = locate(index, pattern.begin(), pattern.end());
    const auto n = locations_.size();
    n_total += n;
    std::vector<int> locations;
    locations.reserve(n);
    for (const auto& x: locations_) {
      locations.push_back(x);
    }
    all_locations.push_back(locations);
  }
  IntegerVector pattern_indices(n_total);
  IntegerVector library_indices(n_total);
  IntegerVector positions(n_total);
  int i_total = 0;
  for (int pattern_idx = 0; pattern_idx < patterns.size(); pattern_idx++) {
    const auto locations = all_locations[pattern_idx];
    for (const auto& location: locations) {
      const auto library_index = std::distance(
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

void FMIndex::load_file(const String& path) {
  std::ifstream in_file(path, std::ios::binary);
  cereal::BinaryInputArchive archive(in_file);
  archive(*this);
}

void FMIndex::save_file(const String& path) {
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

FMIndex* unwrap_index(const List& index) {
  if (as<std::string>(index.attr("class")) != "fmindex")
    stop("Not an FMIndex");
  auto* index_ptr = (FMIndex*) R_ExternalPtrAddr(index["index"]);
  if (index_ptr == NULL)
    stop("Index invalid");
  return index_ptr;
}

//' FM Index functions
//'
//' FM indices are data structures for memory efficient storage of large
//' sets of strings. Searches for partial matches of query strings within the
//' set of strings in the index are extremely fast.
//'
//' \itemize{
//' \item \code{fm_index_construct} Construct new FM Index
//' \item \code{fm_index_find} Find given queries in FM Index
//' \item \code{fm_index_save} Save FM Index to disk
//' \item \code{fm_index_load} Load FM Index from disk
//' }
//'
//' @param strings Vector of strings to construct FM index from
//' @param query Vector of strings to find in FM index
//' @param case_sensitive Build case-sensitive index if TRUE
//' @param index Index created with [fm_index_construct()]
//' @param path Path where to load index from or save index to
//' @examples
//'
//' data("state")
//'
//' index <- fm_index_construct(state.name, case_sensitive = FALSE)
//' # Find all states with "new" in their names
//' hits <- fm_index_find("new", index)
//' hits
//' # Show matching strings in library
//' state.name[hits$library_index]
//'
//' hits <- fm_index_find("ar", index)
//' hits
//' state.name[hits$library_index]
//'
//' tmp_path <- tempfile()
//' fm_index_save(index, tmp_path)
//' index_2 <- fm_index_load(tmp_path)
//'
//' identical(
//'   fm_index_find("new", index),
//'   fm_index_find("new", index_2)
//' )
//'
//' @rdname fmindex
//' @export
//' @importFrom stringi stri_trans_tolower
// [[Rcpp::export]]
List fm_index_construct(CharacterVector strings, bool case_sensitive = false) {
  if (!case_sensitive)
    strings = stri_trans_tolower(strings);
  auto* fm_index = new FMIndex(strings);
  return wrap_index(fm_index);
}

//' @rdname fmindex
//' @export
// [[Rcpp::export]]
DataFrame fm_index_find(const CharacterVector& query, const List& index) {
  return unwrap_index(index)->find(query);
}

//' @rdname fmindex
//' @export
// [[Rcpp::export]]
void fm_index_save(const List& index, const String& path) {
  unwrap_index(index)->save_file(path);
}

//' @rdname fmindex
//' @export
// [[Rcpp::export]]
List fm_index_load(const String& path) {
  auto* fm_index = new FMIndex();
  fm_index->load_file(path);
  return wrap_index(fm_index);
}
