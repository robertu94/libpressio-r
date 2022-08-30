#!/usr/bin/env Rscript

#load the data
input_r <- matrix(data=1, 100,100)
input_lp <- libpressio::data_from_R_typed(input_r, libpressio::DType.float)
compressed_lp <- libpressio::data_new_empty(libpressio::DType.byte, numeric(0))
decompressed_lp <- libpressio::data_new_clone(input_lp)

#load the compressor
lib <- libpressio::get_instance()
pressio <- libpressio::get_compressor(lib, "pressio")

#configure the meta-compressor
pressio_opts <- libpressio::compressor_get_options(pressio)
pressio_opts_R <- libpressio::options_to_R(pressio_opts)
pressio_opts_R$`pressio:compressor` <- "sz"
pressio_opts_R$`pressio:metric` <- "composite"
pressio_opts <- libpressio::options_from_R_typed(pressio_opts_R, pressio_opts)
libpressio::compressor_set_options(pressio, pressio_opts)

#configure the compressor
pressio_opts <- libpressio::compressor_get_options(pressio)
pressio_opts_R <- libpressio::options_to_R(pressio_opts)
pressio_opts_R$`pressio:abs` <- 1e-4
pressio_opts_R$`composite:plugins` <- c("time", "size")
pressio_opts <- libpressio::options_from_R_typed(pressio_opts_R, pressio_opts)
libpressio::compressor_set_options(pressio, pressio_opts)

#run the compressor
libpressio::compressor_compress(pressio, input_lp, compressed_lp)
libpressio::compressor_decompress(pressio, compressed_lp, decompressed_lp)

#get metrics
metrics <- libpressio::options_to_R(libpressio::compressor_get_metrics_results(pressio))
print(metrics)

#access decompressed data
decompressed_r <- libpressio::data_to_R(decompressed_lp)
