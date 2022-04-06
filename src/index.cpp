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
  DataFrame locate(const CharacterVector& patterns);
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

DataFrame FMIndex::locate(const CharacterVector& patterns) {
  std::vector< std::vector<int> > all_locations;
  int n_total = 0;
  for (const auto& pattern: patterns) {
    const auto locations_ = sdsl::locate(index, pattern.begin(), pattern.end());
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
    Named("corpus_index") = library_indices,
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

//' Construct new FM Index
//'
//' FM indices are data structures for memory efficient storage of large
//' sets of strings (corpus). Searches for partial matches with the corpus are
//' extremely fast.
//'
//' @param strings Vector of strings (corpus) to construct FM index from
//' @param case_sensitive Build case-sensitive index if TRUE
//' @return A FM Index object that can be passed to [fm_index_locate()] for
//'  finding matches in the corpus.
//'
//' @examples
//' data("state")
//' index <- fm_index_create(state.name, case_sensitive = FALSE)
//'
//' @family FM Index functions
//' @export
//' @importFrom stringi stri_trans_tolower
// [[Rcpp::export]]
List fm_index_create(CharacterVector strings, bool case_sensitive = false) {
  if (!case_sensitive)
    strings = stri_trans_tolower(strings);
  auto* fm_index = new FMIndex(strings);
  return wrap_index(fm_index);
}

//' Locate given patterns
//'
//' Finds all occurrences of all given patterns in the FM Index, analogous to
//' [stringi::stri_locate()] and `str_locate()` from `stringr`.
//'
//' @param patterns Vector of strings to look for in the index
//' @param index Index created with [fm_index_create()]
//' @return A data frame with three columns. `pattern_index` is the index
//'   of the query pattern, `corpus_index` is the index of the matching
//'   string in the corpus, and `position` is the starting position of the
//'   match within the corpus string. All indices are 1-based.
//'
//' @examples
//' data("state")
//' index <- fm_index_create(state.name, case_sensitive = FALSE)
//' # Find all states with "new" in their names
//' hits <- fm_index_locate("new", index)
//' hits
//' # Show matching strings in library
//' state.name[hits$library_index]
//'
//' hits <- fm_index_locate("ar", index)
//' hits
//' state.name[hits$library_index]
//'
//' @family FM Index functions
//' @export
// [[Rcpp::export]]
DataFrame fm_index_locate(const CharacterVector& patterns, const List& index) {
  return unwrap_index(index)->locate(patterns);
}

//' Save / load FM indices
//'
//' FM indices can be stored on disk and loaded into memory again in order
//' to avoid re-computing the index every time a new R session is opened.
//'
//' @param index FM Index to be saved to disk
//' @param path Path where to save index to or load index from
//'
//' @return
//' For `fm_index_load`, a FM Index object that can be passed to [fm_index_locate()] for
//' finding matches in the corpus.
//'
//' For `fm_index_save`, no return value. Called for side-effects.
//'
//' @examples
//' data("state")
//' index_1 <- fm_index_create(state.name, case_sensitive = FALSE)
//'
//' tmp_path <- tempfile()
//' fm_index_save(index_1, tmp_path)
//' index_2 <- fm_index_load(tmp_path)
//'
//' identical(
//'   fm_index_locate("new", index_1),
//'   fm_index_locate("new", index_2)
//' )
//'
//' @describeIn fm_index_save Save FM Index to disk
//' @family FM Index functions
//' @export
// [[Rcpp::export]]
void fm_index_save(const List& index, const String& path) {
  unwrap_index(index)->save_file(path);
}

//' @describeIn fm_index_save Load FM Index from disk
//' @export
// [[Rcpp::export]]
List fm_index_load(const String& path) {
  auto* fm_index = new FMIndex();
  fm_index->load_file(path);
  return wrap_index(fm_index);
}
