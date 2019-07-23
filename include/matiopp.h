// Copyright 2019 bmc::labs GmbH.
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
        updatecardinality_(*(matvar->dims));
        absorbdata_(matvar);
      }
    } else {
      boost::regex matcher{regex,
                           boost::regex::ECMAScript | boost::regex::icase};

      while ((matvar =
                matvar_type{Mat_VarReadNextInfo(matfp.get()), Mat_VarFree})) {
        if (boost::regex_search(matvar->name, matcher)) {
          updatecardinality_(*(matvar->dims));
          absorbdata_(
            matvar_type{Mat_VarRead(matfp.get(), matvar->name), Mat_VarFree});
        }
      }

      if (data_.empty()) {
        throw std::invalid_argument{"no channels matched"};
      }
    }
  }

  std::vector<key_type> keys() const noexcept {
    std::vector<mat::key_type> keys;
    for (const auto & pair : data_) { keys.push_back(pair.first); }
    return keys;
  }

  std::vector<key_type> channels() const noexcept { return keys(); }

  mapped_type         at(const key_type & key) { return data_.at(key); }
  const mapped_type & at(const key_type & key) const { return data_.at(key); }

  mapped_type         operator[](const key_type & key) { return at(key); }
  const mapped_type & operator[](const key_type & key) const {
    return at(key);
  }

  mapped_type         channel(const key_type & key) { return at(key); }
  const mapped_type & channel(const key_type & key) const { return at(key); }

  const_iterator find(const std::string & regex) const {
    boost::regex matcher{regex,
                         boost::regex::ECMAScript | boost::regex::icase};
    for (auto it = std::begin(data_); it != std::end(data_); ++it) {
      if (boost::regex_search(it->first, matcher)) { return it; }
    }
    return std::end(data_);
  }

  iterator       begin() noexcept { return std::begin(data_); }
  const_iterator cbegin() const noexcept { return std::cbegin(data_); }
  iterator       end() noexcept { return std::end(data_); }
  const_iterator cend() const noexcept { return std::cend(data_); }

  bool        empty() const noexcept { return data_.empty(); }
  std::size_t size() const noexcept { return cardinality_; }
  std::size_t number_of_channels() const noexcept { return data_.size(); }

  bool operator==(const mat & rhs) const { return data_ == rhs.data_; }
  bool operator!=(const mat & rhs) const { return !this->operator==(rhs); }

private:
  void updatecardinality_(std::size_t cardinality) {
    if (cardinality_ == 0u) {
      cardinality_ = cardinality;
    } else if (cardinality_ != cardinality) {
      throw std::length_error{"columns differ in size"};
    }
  }

  void absorbdata_(const matvar_type & matvar) {
    auto data_ptr{static_cast<float *>(matvar->data)};
    data_[matvar->name] = {data_ptr, data_ptr + cardinality_};
  }

private:
  data_type   data_;
  std::size_t cardinality_{0};
};

}  // namespace bmc

#endif  // BMC_INCLUDE_MAT_H_
