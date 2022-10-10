#ifndef LIBPRESSIO_TYPES_H_MOSXN14P
#define LIBPRESSIO_TYPES_H_MOSXN14P
#include <libpressio.h>
#include <libpressio_ext/io/pressio_io.h>
#include <libpressio_ext/cpp/libpressio.h>
#include <Rcpp.h>

// [[Rcpp::plugins("cpp11")]]

RCPP_EXPOSED_CLASS_NODECL(pressio_dtype);
RCPP_EXPOSED_CLASS_NODECL(pressio_thread_safety);
RCPP_EXPOSED_CLASS_NODECL(pressio_option_type);

using options_xptr = Rcpp::XPtr<pressio_options, Rcpp::PreserveStorage, pressio_options_free, true>;
using data_xptr = Rcpp::XPtr<pressio_data, Rcpp::PreserveStorage, pressio_data_free, true>;
using compressor_xptr = Rcpp::XPtr<pressio_compressor, Rcpp::PreserveStorage, pressio_compressor_release, true>;
using library_xptr = Rcpp::XPtr<pressio, Rcpp::PreserveStorage, pressio_release, true>;
using io_xptr = Rcpp::XPtr<pressio_io, Rcpp::PreserveStorage, pressio_io_free, true>;
#endif /* end of include guard: LIBPRESSIO_TYPES_H_MOSXN14P */
