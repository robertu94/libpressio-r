#include "pressio_version.h"
#include "libpressio_types.h"
#include "libpressio_ext/io/posix.h"
#include <sstream>
#include <stdexcept>
#include <array>
#include <algorithm>
#include <vector>
#include <string>
#include <iterator>

void valid_dtype(int type) {
  static const pressio_dtype types[] = {
    pressio_double_dtype,
    pressio_float_dtype,
    pressio_uint8_dtype,
    pressio_uint16_dtype,
    pressio_uint32_dtype,
    pressio_uint64_dtype,
    pressio_int8_dtype,
    pressio_int16_dtype,
    pressio_int32_dtype,
    pressio_int64_dtype,
    pressio_byte_dtype,
  };

  if(std::find(std::begin(types), std::end(types), type) == std::end(types)) {
    throw std::domain_error("invalid value for pressio_dtype enum");
  }
}


//required dummy class used by the enum for pressio_dtypes
class PressioType{
  public:
    PressioType(){}
    void setType(pressio_dtype t) { this->t = t; }
    pressio_dtype getType() const { return t; }
  private:
    pressio_dtype t;
};

//required dummy class used by the enum for pressio_option_types
class PressioOptionType{
  public:
    PressioOptionType(){}
    void setType(pressio_option_type t) { this->t = t; }
    pressio_option_type getType() const { return t; }
  private:
    pressio_option_type t;
};

std::vector<std::string> split(std::string const& supported) {
  std::istringstream ss(supported);
  return std::vector<std::string> (
      std::istream_iterator<std::string>(ss),
      std::istream_iterator<std::string>()
      );
}

// [[Rcpp::export]]
library_xptr get_instance() {
  return library_xptr(pressio_instance(), true);
}

// [[Rcpp::export]]
compressor_xptr get_compressor(library_xptr& library, std::string const& name) {
  pressio_compressor ptr = library->get_compressor(name);
  if(ptr) {
    return compressor_xptr(new pressio_compressor(std::move(ptr)), true);
  } else {
    throw std::runtime_error(library->err_msg());
  }
}

// [[Rcpp::export]]
io_xptr get_io(library_xptr& library, std::string const& name) {
  pressio_io ptr = library->get_io(name);
  if(ptr) {
    return io_xptr(new pressio_io(std::move(ptr)), true);
  } else {
    throw std::runtime_error(library->err_msg());
  }
}

// [[Rcpp::export]]
void compressor_compress(compressor_xptr& compressor, data_xptr const& input, data_xptr& output) {
  auto ret = pressio_compressor_compress(compressor.get(), input.get(), output.get());
  if(ret) {
    throw std::runtime_error(compressor->plugin->error_msg());
  }
}

// [[Rcpp::export]]
void compressor_decompress(compressor_xptr& compressor, data_xptr const& input, data_xptr& output) {
  auto ret = pressio_compressor_decompress(compressor.get(), input.get(), output.get());
  if(ret) {
    throw std::runtime_error(compressor->plugin->error_msg());
  }
}

// [[Rcpp::export]]
options_xptr compressor_get_options(compressor_xptr const& compressor) {
  return options_xptr(new pressio_options(compressor->plugin->get_options()), true);
}


// [[Rcpp::export]]
options_xptr compressor_get_configuration(compressor_xptr const& compressor) {
  return options_xptr(new pressio_options(compressor->plugin->get_configuration()), true);
}

// [[Rcpp::export]]
options_xptr compressor_get_metrics_results(compressor_xptr const& compressor) {
  return options_xptr(new pressio_options(compressor->plugin->get_metrics_results()), true);
}

// [[Rcpp::export]]
void compressor_set_options(compressor_xptr& compressor, options_xptr const& options) {
  auto ret = compressor->plugin->set_options(*options.get());
  if(ret) {
    throw std::runtime_error(compressor->plugin->error_msg());
  }
}

// [[Rcpp::export]]
data_xptr io_read_with_template(io_xptr& io, data_xptr data_template) {
  auto ret = io->plugin->read(data_template.get());
  if(ret) {
    return data_xptr(ret, true);
  } else {
    throw std::runtime_error(io->plugin->error_msg());
  }
}

// [[Rcpp::export]]
void io_write(io_xptr& io, data_xptr data) {
  auto ret = io->plugin->write(data.get());
  if(ret) {
    throw std::runtime_error(io->plugin->error_msg());
  }
}

// [[Rcpp::export]]
data_xptr io_read(io_xptr& io) {
  auto ret = io->plugin->read(nullptr);
  if(ret) {
    return data_xptr(ret, true);
  } else {
    throw std::runtime_error(io->plugin->error_msg());
  }
}

// [[Rcpp::export]]
options_xptr io_get_options(io_xptr const& io) {
  return options_xptr(new pressio_options(io->plugin->get_options()), true);
}


// [[Rcpp::export]]
void io_set_options(io_xptr& io, options_xptr const& options) {
  auto ret = io->plugin->set_options(*options.get());
  if(ret) {
    throw std::runtime_error(io->plugin->error_msg());
  }
}


// [[Rcpp::export]]
Rcpp::CharacterVector supported_compressors() {
  return Rcpp::wrap(split(pressio::supported_compressors()));
}

// [[Rcpp::export]]
Rcpp::CharacterVector supported_metrics() {
  return Rcpp::wrap(split(pressio::supported_metrics()));
}

// [[Rcpp::export]]
Rcpp::CharacterVector supported_io() {
  return Rcpp::wrap(split(pressio::supported_io()));
}

// [[Rcpp::export]]
Rcpp::NumericVector data_to_R(data_xptr const& ptr) {
  auto dims = ptr->dimensions();
  auto data = ptr->to_vector<double>();
  Rcpp::NumericVector data_r = Rcpp::wrap(std::move(data));

  if(dims.size() >= 2) {
    data_r.attr("dim") = Rcpp::NumericVector(dims.begin(), dims.end());
  }
  
  return data_r;
}

template <class NumberVector, class NativeType>
data_xptr data_from_R_number_vector(Rcpp::RObject const& obj, compat::optional<pressio_dtype> dtype = compat::nullopt) {
  NumberVector const& v = Rcpp::as<NumberVector>(obj);
  std::vector<size_t> dims;
  if(obj.hasAttribute("dim")) {
    Rcpp::IntegerVector const& r_dim = Rcpp::as<Rcpp::IntegerVector>(obj.attr("dim"));
    dims.insert(dims.end(), r_dim.begin(), r_dim.end());
  } else {
    dims.push_back(v.length());
  }

  auto data = Rcpp::as<std::vector<NativeType>>(v);
  if(dtype) {
    return data_xptr( new pressio_data(pressio_data::copy(
        pressio_dtype_from_type<NativeType>(),
        data.data(),
        dims
        ).cast(dtype.value())), true);
  } else {
    return data_xptr( new pressio_data(pressio_data::copy(
        pressio_dtype_from_type<NativeType>(),
        data.data(),
        dims
        )), true);
  }
}

// [[Rcpp::export]]
data_xptr data_new_empty(int dtype, std::vector<int> dims) {
  valid_dtype(dtype);
  std::vector<size_t> pdims(dims.begin(), dims.end());
  return data_xptr(pressio_data_new_empty(static_cast<pressio_dtype>(dtype), pdims.size(), pdims.data()), true);
}

// [[Rcpp::export]]
data_xptr data_new_owning(int dtype, std::vector<int> dims) {
  valid_dtype(dtype);
  std::vector<size_t> pdims(dims.begin(), dims.end());
  return data_xptr(pressio_data_new_owning(static_cast<pressio_dtype>(dtype), pdims.size(), pdims.data()), true);
}

// [[Rcpp::export]]
data_xptr io_data_path_read(data_xptr& dims, std::string const& filepath) {
  pressio_data* cloned = new pressio_data(std::move(*dims));
  return data_xptr(pressio_io_data_path_read(cloned, filepath.c_str()), true);
}

// [[Rcpp::export]]
data_xptr data_new_clone(data_xptr const& ptr) {
  return data_xptr(new pressio_data(pressio_data::clone(*ptr)), true);
}

// [[Rcpp::export]]
data_xptr data_from_R(Rcpp::RObject obj) {
  if(Rcpp::is<Rcpp::NumericVector>(obj)) {
    return data_from_R_number_vector<Rcpp::NumericVector, double>(obj);
  } else if(Rcpp::is<Rcpp::IntegerVector>(obj)) {
    return data_from_R_number_vector<Rcpp::IntegerVector, int>(obj);
  } else {
    throw std::runtime_error("unsupported R object");
  }
}
// [[Rcpp::export]]
data_xptr data_from_R_typed(Rcpp::RObject obj, int type) {
  valid_dtype(type);

  if(Rcpp::is<Rcpp::NumericVector>(obj)) {
    return data_from_R_number_vector<Rcpp::NumericVector, double>(obj, static_cast<pressio_dtype>(type));
  } else if(Rcpp::is<Rcpp::IntegerVector>(obj)) {
    return data_from_R_number_vector<Rcpp::IntegerVector, int>(obj, static_cast<pressio_dtype>(type));
  } else {
    throw std::runtime_error("unsupported R object");
  }
}


template <class NumberVector, class NativeType>
pressio_option option_from_R_number_vector(Rcpp::RObject const& obj) {
    NumberVector const& v = Rcpp::as<NumberVector>(obj);
    if(v.length() == 0) {
      return pressio_option();
    } else if(v.length() == 1) {
      return pressio_option(v[0]);
    } else {
      if (v.hasAttribute("dim")) {
        auto r_dim = Rcpp::as<std::vector<int>>(obj.attr("dim"));
        std::vector<size_t> dim(r_dim.begin(), r_dim.end());
        std::vector<NativeType> data(v.begin(), v.end());
        return pressio_data::copy(
            pressio_dtype_from_type<NativeType>(),
            data.data(),
            dim
            );
      } else {
        return pressio_data(v.begin(), v.end());
      }
    }
}
pressio_option option_from_R_impl(Rcpp::RObject const& obj) {
  if(Rcpp::is<Rcpp::NumericVector>(obj)) {
    return option_from_R_number_vector<Rcpp::NumericVector, double>(obj);
  } else if(Rcpp::is<Rcpp::IntegerVector>(obj)) {
    return option_from_R_number_vector<Rcpp::IntegerVector, int>(obj);
  } else if(Rcpp::is<Rcpp::CharacterVector>(obj)) {
    Rcpp::CharacterVector const& v = Rcpp::as<Rcpp::CharacterVector>(obj);
    if(v.length() == 0) {
      return pressio_option();
    } else if(v.length() == 1) {
      return Rcpp::as<std::string>(v.at(0));
    } else {
      auto strings = Rcpp::as<std::vector<std::string>>(v);
      return strings;
    }
  } else if (obj.isNULL()) {
    return pressio_option();
  } else {
    throw std::runtime_error("type not supported ");
  }
}
options_xptr options_from_R_impl(Rcpp::List const& l, pressio_options const* option_types) {
  pressio_options config = (option_types == nullptr)? pressio_options{} : *option_types ;

  auto names = Rcpp::as<std::vector<std::string>>(l.names());
  for (auto const& name : names) {
    Rcpp::RObject const& entry = l[name];
    try {
      pressio_option option = option_from_R_impl(entry);
      if(not option.has_value()) continue;
      if(option_types != nullptr) {
        auto status = config.cast_set(name, option, pressio_conversion_special);
        switch(status) {
          case pressio_options_key_set:
            break;
          case pressio_options_key_exists:
            throw std::runtime_error("invalid type ");
          case pressio_options_key_does_not_exist:
            throw std::runtime_error("does not exist ");
        }
      } else {
        config.set(name, option);
      }
    } catch(std::runtime_error const& ex) {
      throw std::runtime_error(ex.what() + name);
    }
  }

  return options_xptr(new pressio_options(std::move(config)), true);
}

// [[Rcpp::export]]
options_xptr options_from_R_typed(Rcpp::List const& l, options_xptr const& option_types) {
  return options_from_R_impl(l, option_types.get());
}

// [[Rcpp::export]]
options_xptr options_from_R(Rcpp::List const & l) {
  return options_from_R_impl(l, nullptr);
}

#if LIBPRESSIO_HAS_JSON
#include <libpressio_ext/json/pressio_options_json.h>
// [[Rcpp::export]]
options_xptr options_from_json(std::string const& json_str) {
  pressio library;
  auto ptr = pressio_options_new_json(&library, json_str.c_str());
  if(ptr) {
    return options_xptr(ptr, true);
  } else {
    throw std::runtime_error(library.err_msg());
  }
}
#endif

// [[Rcpp::export]]
Rcpp::List options_to_R(options_xptr& ptr) {
  Rcpp::List ret;
  for (auto const& i : *ptr) {
    if(not i.second.has_value()) {
        ret.push_back(R_NilValue, i.first);
        continue;
    }
    switch(i.second.type()) {
      case pressio_option_int8_type:
        ret.push_back(int32_t(i.second.get_value<int8_t>()), i.first);
        break;
      case pressio_option_int16_type:
        ret.push_back(int32_t(i.second.get_value<int16_t>()), i.first);
        break;
      case pressio_option_int32_type:
        ret.push_back(i.second.get_value<int32_t>(), i.first);
        break;
      case pressio_option_int64_type:
        ret.push_back(i.second.get_value<int64_t>(), i.first);
        break;
      case pressio_option_uint8_type:
        ret.push_back(uint32_t(i.second.get_value<uint8_t>()), i.first);
        break;
      case pressio_option_uint16_type:
        ret.push_back(uint32_t(i.second.get_value<uint16_t>()), i.first);
        break;
      case pressio_option_uint32_type:
        ret.push_back(i.second.get_value<uint32_t>(), i.first);
        break;
      case pressio_option_uint64_type:
        ret.push_back(i.second.get_value<uint64_t>(), i.first);
        break;
      case pressio_option_float_type:
        ret.push_back(i.second.get_value<float>(), i.first);
        break;
      case pressio_option_double_type:
        ret.push_back(i.second.get_value<double>(), i.first);
        break;
      case pressio_option_charptr_type:
        ret.push_back(i.second.get_value<std::string>(), i.first);
        break;
      case pressio_option_charptr_array_type:
        {
        auto strings = i.second.get_value<std::vector<std::string>>();
        ret.push_back(std::move(strings), i.first);
        }
        break;
      case pressio_option_data_type:
        {
          auto data = i.second.get_value<pressio_data>();
          auto dims = data.dimensions();
          if(not dims.empty()) {
            auto data_casted = data.to_vector<double>();
            Rcpp::IntegerVector r_dims = Rcpp::wrap(dims);
            Rcpp::NumericVector r_data = Rcpp::wrap(std::move(data_casted));
            r_data.attr("dim") = r_dims;
            ret.push_back(std::move(r_data), i.first);
          } else {
            ret.push_back(Rcpp::NumericVector(), i.first);
          }
          break;
        }
      case pressio_option_userptr_type:
        //silently ignore these values
        continue;
      case pressio_option_unset_type:
        throw std::runtime_error("failed to convert option: " + i.first);
        //throw std::runtime_error("failed to convert option: " + i.first);
    }
  }

  return ret;
}

// [[Rcpp::export]]
std::string options_to_string(options_xptr& ptr) {
  std::stringstream ss;
  ss << *ptr;
  return ss.str();
}

// [[Rcpp::export]]
std::string data_to_string(data_xptr& ptr) {
  std::stringstream ss;
  ss << *ptr;
  return ss.str();
}


RCPP_MODULE(pressio) {
  Rcpp::class_<PressioOptionType>("OptionType")
    .constructor()
    .property("type", &PressioOptionType::getType, &PressioOptionType::setType)
    ;
  Rcpp::class_<PressioType>("DType")
    .constructor()
    .property("type", &PressioType::getType, &PressioType::setType)
    ;
  Rcpp::enum_<pressio_option_type, PressioOptionType>("OptionTypeEnum")
    .value("int8t", pressio_option_int8_type)
    .value("int16t", pressio_option_int16_type)
    .value("int32t", pressio_option_int32_type)
    .value("int64t", pressio_option_int64_type)
    .value("uint8t", pressio_option_uint8_type)
    .value("uint16t", pressio_option_uint16_type)
    .value("uint32t", pressio_option_uint32_type)
    .value("int64t", pressio_option_uint64_type)
    .value("float", pressio_option_float_type)
    .value("double", pressio_option_double_type)
    .value("data", pressio_option_data_type)
    .value("string", pressio_option_charptr_type)
    .value("strings", pressio_option_charptr_array_type) .value("userptr", pressio_option_userptr_type)
    .value("unset", pressio_option_unset_type)
    ;
  Rcpp::enum_<pressio_dtype, PressioType>("DTypeEnum")
    .value("int8t", pressio_int8_dtype)
    .value("int16t", pressio_int16_dtype)
    .value("int32t", pressio_int32_dtype)
    .value("int64t", pressio_int64_dtype)
    .value("uint8t", pressio_uint8_dtype)
    .value("uint16t", pressio_uint16_dtype)
    .value("uint32t", pressio_uint32_dtype)
    .value("int64t", pressio_uint64_dtype)
    .value("float", pressio_float_dtype)
    .value("double", pressio_double_dtype)
    .value("byte", pressio_byte_dtype)
    ;

}

