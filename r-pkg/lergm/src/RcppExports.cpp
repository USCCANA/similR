// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include "lergm_types.h"
#include <Rcpp.h>

using namespace Rcpp;

// make_sets
vecint make_sets(int n);
RcppExport SEXP _lergm_make_sets(SEXP nSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< int >::type n(nSEXP);
    rcpp_result_gen = Rcpp::wrap(make_sets(n));
    return rcpp_result_gen;
END_RCPP
}
// powerset
vecvecint powerset(int n, bool force);
RcppExport SEXP _lergm_powerset(SEXP nSEXP, SEXP forceSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< int >::type n(nSEXP);
    Rcpp::traits::input_parameter< bool >::type force(forceSEXP);
    rcpp_result_gen = Rcpp::wrap(powerset(n, force));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_lergm_make_sets", (DL_FUNC) &_lergm_make_sets, 1},
    {"_lergm_powerset", (DL_FUNC) &_lergm_powerset, 2},
    {NULL, NULL, 0}
};

RcppExport void R_init_lergm(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}