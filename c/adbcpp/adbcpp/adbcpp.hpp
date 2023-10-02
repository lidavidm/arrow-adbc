// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

/// \file adbcpp.hpp Header-only C++ helpers for ADBC
///
/// A set of simple helpers for using ADBC types more easily from C++.

#pragma once

#include <cassert>
#include <cstring>

#include <adbc.h>

namespace adbc {

namespace internal {

/// \brief A helper to release different kinds of ADBC resources
///   generically.
template <typename T>
struct Releaser {
  static AdbcStatusCode Release(T* value, struct AdbcError*) {
    if (value->release) {
      value->release(value);
    }
    return ADBC_STATUS_OK;
  }
};

template <>
struct Releaser<struct AdbcConnection> {
  static AdbcStatusCode Release(struct AdbcConnection* value, struct AdbcError* error) {
    if (value->private_data) {
      return AdbcConnectionRelease(value, error);
    }
    return ADBC_STATUS_OK;
  }
};

template <>
struct Releaser<struct AdbcDatabase> {
  static AdbcStatusCode Release(struct AdbcDatabase* value, struct AdbcError* error) {
    if (value->private_data) {
      return AdbcDatabaseRelease(value, error);
    }
    return ADBC_STATUS_OK;
  }
};

template <>
struct Releaser<struct AdbcStatement> {
  static AdbcStatusCode Release(struct AdbcStatement* value, struct AdbcError* error) {
    if (value->private_data) {
      return AdbcStatementRelease(value, error);
    }
    return ADBC_STATUS_OK;
  }
};

}

/// \brief An RAII wrapper around an ADBC resource.
///
/// This will assert if the resource could not be released cleanly.  To avoid
/// this, explicitly call Release() and handle the error.
template <typename Resource>
class Handle {
 public:
  Handle() { std::memset(&value_, 0, sizeof(value_)); }

  ~Handle() {
    AdbcStatusCode status = Release(nullptr);
    (void)status;
    assert(status == ADBC_STATUS_OK);
  }

  Resource* operator->() noexcept { return &value_; }
  const Resource* operator->() const noexcept { return &value_; }
  Resource* get() noexcept { return &value_; }
  const Resource* get() const noexcept { return &value_; }

  /// \brief Release the inner resource.
  AdbcStatusCode Release(struct AdbcError* error) {
    auto status = adbc::internal::Releaser<Resource>::Release(&value_, error);
    if (status == ADBC_STATUS_OK) {
      std::memset(&value_, 0, sizeof(value_));
    }
    return status;
  }

 protected:
  Resource value_;
};

using ConnectionHandle = Handle<struct AdbcConnection>;
using DatabaseHandle = Handle<struct AdbcDatabase>;
using StatementHandle = Handle<struct AdbcStatement>;

// TODO: macros for error handling

} // namespace adbc
