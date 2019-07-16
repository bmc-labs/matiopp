// Copyright 2019 Florian Eich <florian@bmc-labs.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef BMC_INCLUDE_MAT_H_
#define BMC_INCLUDE_MAT_H_

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <matio.h>
#include <boost/regex.hpp>


namespace bmc {


class mat
{
public:
  using key_type       = std::string;
  using mapped_type    = std::vector<float>;
  using matvar_type    = std::shared_ptr<matvar_t>;
  using value_type     = std::pair<const key_type, matvar_type>;
  using data_type      = std::unordered_map<key_type, mapped_type>;
  using size_type      = std::size_t;
  using iterator       = data_type::iterator;
  using const_iterator = data_type::const_iterator;

public:
  explicit mat(const std::string & filename, const std::string & regex = "") {
    std::unique_ptr<mat_t, decltype(&Mat_Close)> matfp{
      Mat_Open(filename.c_str(), MAT_ACC_RDONLY), Mat_Close};

    if (!matfp) { throw std::invalid_argument{"file does not exist"}; }

    matvar_type matvar;
    if (regex.empty()) {
      while (
        (matvar = matvar_type{Mat_VarReadNext(matfp.get()), Mat_VarFree})) {
        update_cardinality(*(matvar->dims));
        absorb_data(matvar);
      }
    } else {
      boost::regex  matcher{regex,
                           boost::regex::ECMAScript | boost::regex::icase};
      boost::smatch match;

      while ((matvar =
                matvar_type{Mat_VarReadNextInfo(matfp.get()), Mat_VarFree})) {
        if (boost::regex_search(matvar->name, matcher)) {
          update_cardinality(*(matvar->dims));
          absorb_data(
            matvar_type{Mat_VarRead(matfp.get(), matvar->name), Mat_VarFree});
        }
      }

      if (_data.empty()) {
        throw std::invalid_argument{"no channels matched"};
      }
    }
  }

  std::vector<key_type> keys() const noexcept {
    std::vector<mat::key_type> keys;
    for (const auto & pair : _data) { keys.push_back(pair.first); }
    return keys;
  }

  std::vector<key_type> channels() const noexcept { return keys(); }

  mapped_type         at(const key_type & key) { return _data.at(key); }
  const mapped_type & at(const key_type & key) const { return _data.at(key); }

  mapped_type         operator[](const key_type & key) { return at(key); }
  const mapped_type & operator[](const key_type & key) const {
    return at(key);
  }

  mapped_type         channel(const key_type & key) { return at(key); }
  const mapped_type & channel(const key_type & key) const { return at(key); }

  const_iterator find(const std::string & regex) const {
    boost::regex matcher{regex,
                         boost::regex::ECMAScript | boost::regex::icase};
    for (auto it = std::begin(_data); it != std::end(_data); ++it) {
      if (boost::regex_search(it->first, matcher)) { return it; }
    }
    return std::end(_data);
  }

  iterator       begin() noexcept { return std::begin(_data); }
  const_iterator cbegin() const noexcept { return std::cbegin(_data); }
  iterator       end() noexcept { return std::end(_data); }
  const_iterator cend() const noexcept { return std::cend(_data); }

  bool        empty() const noexcept { return _data.empty(); }
  std::size_t size() const noexcept { return _cardinality; }
  std::size_t number_of_channels() const noexcept { return _data.size(); }

  bool operator==(const mat & rhs) const { return _data == rhs._data; }
  bool operator!=(const mat & rhs) const { return !this->operator==(rhs); }

private:
  void update_cardinality(std::size_t cardinality) {
    if (_cardinality == 0u) {
      _cardinality = cardinality;
    } else if (_cardinality != cardinality) {
      throw std::length_error{"columns differ in size"};
    }
  }

  void absorb_data(const matvar_type & matvar) {
    auto data_ptr{static_cast<float *>(matvar->data)};
    _data[matvar->name] = {data_ptr, data_ptr + _cardinality};
  }

private:
  data_type   _data;
  std::size_t _cardinality{0};
};

}  // namespace bmc

#endif  // BMC_INCLUDE_MAT_H_
