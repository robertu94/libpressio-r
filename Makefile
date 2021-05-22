VERSION=$(shell awk -F": " '/Version/ {print $$2 }' DESCRIPTION )
all: clean
	R -e "library(Rcpp); Rcpp::compileAttributes(verbose=TRUE);"
	R CMD build .
	R CMD INSTALL libpressio_$(VERSION).tar.gz
check:
	R CMD ./examples/test.R
clean:
	$(RM) -rf inst
dist-clean:
	$(RM) $(wildcard *.tar.gz)
