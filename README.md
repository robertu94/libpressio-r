# LibPressio for R

Bindings for LibPressio in the R language.

These bindings are unstable and may evolve over time as needed.

## Installation

Install Rcpp and LibPressio first.

```cpp
make
```

## Usage

Routines have mostly the same meaning as they do in LibPressio.

```R
#!/usr/bin/env Rscript
library(libpressio)
library(RColorBrewer)

cmap <- rev(colorRampPalette(brewer.pal(10, "RdBu"))(1024))

#get a compressor handle
lib <- libpressio::get_instance()
sz <- libpressio::get_compressor(lib, "sz")
sz_opts <- libpressio::compressor_get_options(sz)

#configure the compressor
sz_opts_R <- libpressio::options_to_R(sz_opts)
sz_opts_R$`sz:abs_err_bound` <- 1e-6
sz_opts_R$`sz:error_bound_mode_str` <- "abs"
sz_opts_R$`sz:metric` <- "error_stat"
sz_opts <- libpressio::options_from_R_typed(sz_opts_R, sz_opts)
libpressio::compressor_set_options(sz, sz_opts)

#load the data
input <- libpressio::io_data_path_read(
              libpressio::data_new_empty(libpressio::DType.float, c(500, 500, 100)),
              "/home/runderwood/git/datasets/hurricane/100x500x500/CLOUDf48.bin.f32"
            )
compressed <- libpressio::data_new_empty(libpressio::DType.byte, numeric(0))
decompressed <- libpressio::data_new_clone(input)

input_r <- libpressio::data_to_R(input)
png(filename="/tmp/uncompressed.png")
image(input_r[,,50], col=cmap, useRaster=TRUE, axes=FALSE)

#run the compressors
libpressio::compressor_compress(sz, input, compressed)
libpressio::compressor_decompress(sz, compressed, decompressed)

#print the metrics
metrics <- libpressio::options_to_R(libpressio::compressor_get_metrics_results(sz))
print(metrics)

#retrieve the decompressed data
decompressed_r <- libpressio::data_to_R(decompressed)
png(filename="/tmp/decompressed.png")
image(decompressed_r[,,50], col=cmap, useRaster=TRUE, axes=FALSE)
```
