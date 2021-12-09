// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// fm_index_construct
SEXP fm_index_construct(CharacterVector strings, bool case_sensitive);
RcppExport SEXP _fm_index_fm_index_construct(SEXP stringsSEXP, SEXP case_sensitiveSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< CharacterVector >::type strings(stringsSEXP);
    Rcpp::traits::input_parameter< bool >::type case_sensitive(case_sensitiveSEXP);
    rcpp_result_gen = Rcpp::wrap(fm_index_construct(strings, case_sensitive));
    return rcpp_result_gen;
END_RCPP
}
// fm_index_find
DataFrame fm_index_find(CharacterVector query, SEXP index);
RcppExport SEXP _fm_index_fm_index_find(SEXP querySEXP, SEXP indexSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< CharacterVector >::type query(querySEXP);
    Rcpp::traits::input_parameter< SEXP >::type index(indexSEXP);
    rcpp_result_gen = Rcpp::wrap(fm_index_find(query, index));
    return rcpp_result_gen;
END_RCPP
}
// fm_index_save
void fm_index_save(SEXP index, String path);
RcppExport SEXP _fm_index_fm_index_save(SEXP indexSEXP, SEXP pathSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type index(indexSEXP);
    Rcpp::traits::input_parameter< String >::type path(pathSEXP);
    fm_index_save(index, path);
    return R_NilValue;
END_RCPP
}
// fm_index_load
SEXP fm_index_load(String path);
RcppExport SEXP _fm_index_fm_index_load(SEXP pathSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< String >::type path(pathSEXP);
    rcpp_result_gen = Rcpp::wrap(fm_index_load(path));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_fm_index_fm_index_construct", (DL_FUNC) &_fm_index_fm_index_construct, 2},
    {"_fm_index_fm_index_find", (DL_FUNC) &_fm_index_fm_index_find, 2},
    {"_fm_index_fm_index_save", (DL_FUNC) &_fm_index_fm_index_save, 2},
    {"_fm_index_fm_index_load", (DL_FUNC) &_fm_index_fm_index_load, 1},
    {NULL, NULL, 0}
};

RcppExport void R_init_fm_index(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
