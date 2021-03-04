all: clean
	R -e "library(Rcpp); Rcpp::compileAttributes(verbose=TRUE);"
	R CMD build .
	R CMD INSTALL libpressio_1.0.tar.gz
clean:
	$(RM) -rf inst
