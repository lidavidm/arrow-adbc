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

#pragma once

#include <charconv>
#include <cstring>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <adbc.h>
#include <fmt/core.h>
#include <fmt/format.h>

#include "driver/common/utils.h"
#include "driver/framework/status.h"

/// \file driver.h ADBC Driver Framework
///
/// A base implementation of an ADBC driver that allows easier driver
/// development just by overriding functions.
namespace adbc::driver {

/// \brief Track the state of a database/connection/statement.
enum class LifecycleState {
  /// \brief New has been called but not Init.
  kUninitialized,
  /// \brief Init has been called.
  kInitialized,
};

/// \brief A typed option value wrapper. It currently does not attempt
/// conversion (i.e., getting a double option as a string).
class Option {
 public:
  /// The option is unset.
  struct NotFound {};
  // TODO: use NotFound instead of std::nullopt, and rename to Unset or something.
  using Value = std::variant<NotFound, std::optional<std::string>, std::vector<uint8_t>,
                             int64_t, double>;

  Option() : value_(NotFound{}) {}
  explicit Option(const char* value)
      : value_(value ? std::make_optional(std::string(value)) : std::nullopt) {}
  explicit Option(std::string value) : value_(std::move(value)) {}
  explicit Option(std::vector<uint8_t> value) : value_(std::move(value)) {}
  explicit Option(double value) : value_(value) {}
  explicit Option(int64_t value) : value_(value) {}

  const Value& value() const& { return value_; }
  Value& value() && { return value_; }

  Result<bool> AsBool() const {
    return std::visit(
        [&](auto&& value) -> Result<bool> {
          using T = std::decay_t<decltype(value)>;
          if constexpr (std::is_same_v<T, std::optional<std::string>>) {
            if (!value.has_value()) {
              return status::InvalidArgument("Invalid boolean value (NULL)");
            } else if (*value == ADBC_OPTION_VALUE_ENABLED) {
              return true;
            } else if (*value == ADBC_OPTION_VALUE_DISABLED) {
              return false;
            }
            return status::InvalidArgument("Invalid boolean value '{}'", *value);
          }
          return status::InvalidArgument("Value must be 'true' or 'false'");
        },
        value_);
  }

  Result<int64_t> AsInt() const {
    return std::visit(
        [&](auto&& value) -> Result<int64_t> {
          using T = std::decay_t<decltype(value)>;
          if constexpr (std::is_same_v<T, int64_t>) {
            return value;
          } else if constexpr (std::is_same_v<T, std::optional<std::string>>) {
            if (value.has_value()) {
              int64_t parsed = 0;
              auto begin = value->data();
              auto end = value->data() + value->size();
              auto result = std::from_chars(begin, end, parsed);
              if (result.ec != std::errc()) {
                return status::InvalidArgument("Value is not an integer: {}", *value);
              } else if (result.ptr != end) {
                return status::InvalidArgument("Value has trailing data: {}", *value);
              }
              return parsed;
            }
          }
          return status::InvalidArgument("Value must be an integer");
        },
        value_);
  }

  Result<std::optional<std::string>> AsString() const {
    return std::visit(
        [&](auto&& value) -> Result<std::optional<std::string>> {
          using T = std::decay_t<decltype(value)>;
          if constexpr (std::is_same_v<T, std::optional<std::string>>) {
            return value;
          }
          return status::InvalidArgument("Value must be a string");
        },
        value_);
  }

 private:
  Value value_;

  // Methods used by trampolines to export option values in C below
  friend class ObjectBase;

  AdbcStatusCode CGet(char* out, size_t* length) const {
    // TODO: no way to return error
    if (!out || !length) {
      return ADBC_STATUS_INVALID_ARGUMENT;
    }
    return std::visit(
        [&](auto&& value) {
          using T = std::decay_t<decltype(value)>;
          if constexpr (std::is_same_v<T, std::optional<std::string>>) {
            if (!value) {
              *length = 0;
              return ADBC_STATUS_OK;
            }
            size_t value_size_with_terminator = value->size() + 1;
            if (*length >= value_size_with_terminator) {
              std::memcpy(out, value->data(), value->size());
              out[value->size()] = 0;
            }
            *length = value_size_with_terminator;
            return ADBC_STATUS_OK;
          } else {
            return ADBC_STATUS_NOT_FOUND;
          }
        },
        value_);
  }

  AdbcStatusCode CGet(uint8_t* out, size_t* length) const {
    if (!out || !length) {
      return ADBC_STATUS_INVALID_ARGUMENT;
    }
    return std::visit(
        [&](auto&& value) {
          using T = std::decay_t<decltype(value)>;
          if constexpr (std::is_same_v<T, std::optional<std::string>>) {
            if (!value) {
              *length = 0;
              return ADBC_STATUS_OK;
            }
            size_t value_size_with_terminator = value->size() + 1;
            if (*length >= value_size_with_terminator) {
              std::memcpy(out, value->data(), value->size());
              out[value->size()] = 0;
            }
            *length = value_size_with_terminator;
            return ADBC_STATUS_OK;
          } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
            if (*length >= value.size()) {
              std::memcpy(out, value.data(), value.size());
            }
            *length = value.size();
            return ADBC_STATUS_OK;
          } else {
            return ADBC_STATUS_NOT_FOUND;
          }
        },
        value_);
  }

  AdbcStatusCode CGet(int64_t* out) const {
    if (!out) {
      return ADBC_STATUS_INVALID_ARGUMENT;
    }
    return std::visit(
        [&](auto&& value) {
          using T = std::decay_t<decltype(value)>;
          if constexpr (std::is_same_v<T, int64_t>) {
            *out = value;
            return ADBC_STATUS_OK;
          } else {
            return ADBC_STATUS_NOT_FOUND;
          }
        },
        value_);
  }

  AdbcStatusCode CGet(double* out) const {
    if (!out) {
      return ADBC_STATUS_INVALID_ARGUMENT;
    }
    return std::visit(
        [&](auto&& value) {
          using T = std::decay_t<decltype(value)>;
          if constexpr (std::is_same_v<T, double> || std::is_same_v<T, int64_t>) {
            *out = value;
            return ADBC_STATUS_OK;
          } else {
            return ADBC_STATUS_NOT_FOUND;
          }
        },
        value_);
  }
};

// Base class for private_data of AdbcDatabase, AdbcConnection, and AdbcStatement
// This class handles option setting and getting.
class ObjectBase {
 public:
  ObjectBase() {}

  virtual ~ObjectBase() = default;

  // Called After zero or more SetOption() calls. The parent is the
  // private_data of the AdbcDatabase, or AdbcConnection when initializing a
  // subclass of ConnectionObjectBase, and StatementObjectBase (respectively),
  // or otherwise nullptr.  For example, if you have defined
  // Driver<MyDatabase, MyConnection, MyStatement>, you can
  // reinterpret_cast<MyDatabase>(parent) in MyConnection::Init().
  virtual AdbcStatusCode Init(void* parent, AdbcError* error) { return ADBC_STATUS_OK; }

  // Called when the corresponding AdbcXXXRelease() function is invoked from C.
  // Driver authors can override this method to return an error if the object is
  // not in a valid state (e.g., if a connection has open statements) or to clean
  // up resources when resource cleanup could fail. Resource cleanup that cannot fail
  // (e.g., releasing memory) should generally be handled in the deleter.
  virtual AdbcStatusCode Release(AdbcError* error) { return ADBC_STATUS_OK; }

  // Get an option value.
  virtual Result<std::optional<Option>> GetOption(std::string_view key) const {
    return std::nullopt;
  }

  // Driver authors can override this method to reject options that are not supported or
  // that are set at a time not supported by the driver (e.g., to reject options that are
  // set after Init() is called if this is not supported).
  virtual AdbcStatusCode SetOption(std::string_view key, Option value, AdbcError* error) {
    return ADBC_STATUS_NOT_IMPLEMENTED;
  }

 private:
  // Let the Driver use these to expose C callables wrapping option setters/getters
  template <typename DatabaseT, typename ConnectionT, typename StatementT>
  friend class Driver;

  template <typename T>
  AdbcStatusCode CSetOption(const char* key, T value, AdbcError* error) {
    Option option(value);
    return SetOption(key, std::move(option), error);
  }

  AdbcStatusCode CSetOptionBytes(const char* key, const uint8_t* value, size_t length,
                                 AdbcError* error) {
    std::vector<uint8_t> cppvalue(value, value + length);
    Option option(std::move(cppvalue));
    return SetOption(key, std::move(option), error);
  }

  template <typename T>
  AdbcStatusCode CGetOptionStringLike(const char* key, T* value, size_t* length,
                                      AdbcError* error) const {
    RAISE_RESULT(error, auto option, GetOption(key));
    if (option.has_value()) {
      // TODO: pass error through
      return option->CGet(value, length);
    } else {
      SetError(error, "option '%s' not found", key);
      return ADBC_STATUS_NOT_FOUND;
    }
  }

  template <typename T>
  AdbcStatusCode CGetOptionNumeric(const char* key, T* value, AdbcError* error) const {
    RAISE_RESULT(error, auto option, GetOption(key));
    if (option.has_value()) {
      // TODO: pass error through
      return option->CGet(value);
    } else {
      SetError(error, "option '%s' not found", key);
      return ADBC_STATUS_NOT_FOUND;
    }
  }
};

/// Helper for below: given the ADBC type, pick the right driver type.
template <typename DatabaseT, typename ConnectionT, typename StatementT, typename T>
struct ResolveObjectTImpl {};

template <typename DatabaseT, typename ConnectionT, typename StatementT>
struct ResolveObjectTImpl<DatabaseT, ConnectionT, StatementT, struct AdbcDatabase> {
  using type = DatabaseT;
};
template <typename DatabaseT, typename ConnectionT, typename StatementT>
struct ResolveObjectTImpl<DatabaseT, ConnectionT, StatementT, struct AdbcConnection> {
  using type = ConnectionT;
};
template <typename DatabaseT, typename ConnectionT, typename StatementT>
struct ResolveObjectTImpl<DatabaseT, ConnectionT, StatementT, struct AdbcStatement> {
  using type = StatementT;
};

/// Helper for below: given the ADBC type, pick the right driver type.
template <typename DatabaseT, typename ConnectionT, typename StatementT, typename T>
using ResolveObjectT =
    typename ResolveObjectTImpl<DatabaseT, ConnectionT, StatementT, T>::type;

// Driver authors can declare a template specialization of the Driver class
// and use it to provide their driver init function. It is possible, but
// rarely useful, to subclass a driver.
template <typename DatabaseT, typename ConnectionT, typename StatementT>
class Driver {
 public:
  static AdbcStatusCode Init(int version, void* raw_driver, AdbcError* error) {
    // TODO: support 1_0_0
    if (version != ADBC_VERSION_1_1_0) return ADBC_STATUS_NOT_IMPLEMENTED;
    AdbcDriver* driver = (AdbcDriver*)raw_driver;
    std::memset(driver, 0, sizeof(AdbcDriver));

    // Driver lifecycle
    driver->private_data = new Driver();
    driver->release = &CDriverRelease;

    // Driver functions
    driver->ErrorGetDetailCount = &CErrorGetDetailCount;
    driver->ErrorGetDetail = &CErrorGetDetail;

    // Database lifecycle
    driver->DatabaseNew = &CNew<AdbcDatabase>;
    driver->DatabaseInit = &CDatabaseInit;
    driver->DatabaseRelease = &CRelease<AdbcDatabase>;

    // Database functions
    driver->DatabaseSetOption = &CSetOption<AdbcDatabase>;
    driver->DatabaseSetOptionBytes = &CSetOptionBytes<AdbcDatabase>;
    driver->DatabaseSetOptionInt = &CSetOptionInt<AdbcDatabase>;
    driver->DatabaseSetOptionDouble = &CSetOptionDouble<AdbcDatabase>;
    driver->DatabaseGetOption = &CGetOption<AdbcDatabase>;
    driver->DatabaseGetOptionBytes = &CGetOptionBytes<AdbcDatabase>;
    driver->DatabaseGetOptionInt = &CGetOptionInt<AdbcDatabase>;
    driver->DatabaseGetOptionDouble = &CGetOptionDouble<AdbcDatabase>;

    // Connection lifecycle
    driver->ConnectionNew = &CNew<AdbcConnection>;
    driver->ConnectionInit = &CConnectionInit;
    driver->ConnectionRelease = &CRelease<AdbcConnection>;

    // Connection functions
    driver->ConnectionSetOption = &CSetOption<AdbcConnection>;
    driver->ConnectionSetOptionBytes = &CSetOptionBytes<AdbcConnection>;
    driver->ConnectionSetOptionInt = &CSetOptionInt<AdbcConnection>;
    driver->ConnectionSetOptionDouble = &CSetOptionDouble<AdbcConnection>;
    driver->ConnectionGetOption = &CGetOption<AdbcConnection>;
    driver->ConnectionGetOptionBytes = &CGetOptionBytes<AdbcConnection>;
    driver->ConnectionGetOptionInt = &CGetOptionInt<AdbcConnection>;
    driver->ConnectionGetOptionDouble = &CGetOptionDouble<AdbcConnection>;
    driver->ConnectionCommit = &CConnectionCommit;
    driver->ConnectionGetInfo = &CConnectionGetInfo;
    driver->ConnectionGetObjects = &CConnectionGetObjects;
    driver->ConnectionGetTableSchema = &CConnectionGetTableSchema;
    driver->ConnectionGetTableTypes = &CConnectionGetTableTypes;
    driver->ConnectionReadPartition = &CConnectionReadPartition;
    driver->ConnectionRollback = &CConnectionRollback;
    driver->ConnectionCancel = &CConnectionCancel;
    driver->ConnectionGetStatistics = &CConnectionGetStatistics;
    driver->ConnectionGetStatisticNames = &CConnectionGetStatisticNames;

    // Statement lifecycle
    driver->StatementNew = &CStatementNew;
    driver->StatementRelease = &CRelease<AdbcStatement>;

    // Statement functions
    driver->StatementSetOption = &CSetOption<AdbcStatement>;
    driver->StatementSetOptionBytes = &CSetOptionBytes<AdbcStatement>;
    driver->StatementSetOptionInt = &CSetOptionInt<AdbcStatement>;
    driver->StatementSetOptionDouble = &CSetOptionDouble<AdbcStatement>;
    driver->StatementGetOption = &CGetOption<AdbcStatement>;
    driver->StatementGetOptionBytes = &CGetOptionBytes<AdbcStatement>;
    driver->StatementGetOptionInt = &CGetOptionInt<AdbcStatement>;
    driver->StatementGetOptionDouble = &CGetOptionDouble<AdbcStatement>;

    driver->StatementExecuteQuery = &CStatementExecuteQuery;
    driver->StatementExecuteSchema = &CStatementExecuteSchema;
    driver->StatementGetParameterSchema = &CStatementGetParameterSchema;
    driver->StatementPrepare = &CStatementPrepare;
    driver->StatementSetSqlQuery = &CStatementSetSqlQuery;
    driver->StatementSetSubstraitPlan = &CStatementSetSubstraitPlan;
    driver->StatementBind = &CStatementBind;
    driver->StatementBindStream = &CStatementBindStream;
    driver->StatementCancel = &CStatementCancel;

    return ADBC_STATUS_OK;
  }

  // Driver trampolines
  static AdbcStatusCode CDriverRelease(AdbcDriver* driver, AdbcError* error) {
    auto driver_private = reinterpret_cast<Driver*>(driver->private_data);
    delete driver_private;
    driver->private_data = nullptr;
    return ADBC_STATUS_OK;
  }

  static int CErrorGetDetailCount(const AdbcError* error) {
    if (error->vendor_code != ADBC_ERROR_VENDOR_CODE_PRIVATE_DATA) {
      return 0;
    }

    auto error_obj = reinterpret_cast<Status*>(error->private_data);
    return error_obj->CDetailCount();
  }

  static AdbcErrorDetail CErrorGetDetail(const AdbcError* error, int index) {
    auto error_obj = reinterpret_cast<Status*>(error->private_data);
    return error_obj->CDetail(index);
  }

  // Templatable trampolines

  template <typename T>
  static AdbcStatusCode CNew(T* obj, AdbcError* error) {
    using ObjectT = ResolveObjectT<DatabaseT, ConnectionT, StatementT, T>;
    auto private_data = new ObjectT();
    obj->private_data = private_data;
    return ADBC_STATUS_OK;
  }

  template <typename T>
  static AdbcStatusCode CRelease(T* obj, AdbcError* error) {
    using ObjectT = ResolveObjectT<DatabaseT, ConnectionT, StatementT, T>;
    if (obj == nullptr) return ADBC_STATUS_INVALID_STATE;
    auto private_data = reinterpret_cast<ObjectT*>(obj->private_data);
    if (private_data == nullptr) return ADBC_STATUS_INVALID_STATE;
    AdbcStatusCode result = private_data->Release(error);
    if (result != ADBC_STATUS_OK) {
      return result;
    }

    delete private_data;
    obj->private_data = nullptr;
    return ADBC_STATUS_OK;
  }

  template <typename T>
  static AdbcStatusCode CSetOption(T* obj, const char* key, const char* value,
                                   AdbcError* error) {
    using ObjectT = ResolveObjectT<DatabaseT, ConnectionT, StatementT, T>;
    auto private_data = reinterpret_cast<ObjectT*>(obj->private_data);
    return private_data->template CSetOption<>(key, value, error);
  }

  template <typename T>
  static AdbcStatusCode CSetOptionBytes(T* obj, const char* key, const uint8_t* value,
                                        size_t length, AdbcError* error) {
    using ObjectT = ResolveObjectT<DatabaseT, ConnectionT, StatementT, T>;
    auto private_data = reinterpret_cast<ObjectT*>(obj->private_data);
    return private_data->CSetOptionBytes(key, value, length, error);
  }

  template <typename T>
  static AdbcStatusCode CSetOptionInt(T* obj, const char* key, int64_t value,
                                      AdbcError* error) {
    using ObjectT = ResolveObjectT<DatabaseT, ConnectionT, StatementT, T>;
    auto private_data = reinterpret_cast<ObjectT*>(obj->private_data);
    return private_data->template CSetOption<>(key, value, error);
  }

  template <typename T>
  static AdbcStatusCode CSetOptionDouble(T* obj, const char* key, double value,
                                         AdbcError* error) {
    using ObjectT = ResolveObjectT<DatabaseT, ConnectionT, StatementT, T>;
    auto private_data = reinterpret_cast<ObjectT*>(obj->private_data);
    return private_data->template CSetOption<>(key, value, error);
  }

  template <typename T>
  static AdbcStatusCode CGetOption(T* obj, const char* key, char* value, size_t* length,
                                   AdbcError* error) {
    using ObjectT = ResolveObjectT<DatabaseT, ConnectionT, StatementT, T>;
    auto private_data = reinterpret_cast<ObjectT*>(obj->private_data);
    return private_data->template CGetOptionStringLike<>(key, value, length, error);
  }

  template <typename T>
  static AdbcStatusCode CGetOptionBytes(T* obj, const char* key, uint8_t* value,
                                        size_t* length, AdbcError* error) {
    using ObjectT = ResolveObjectT<DatabaseT, ConnectionT, StatementT, T>;
    auto private_data = reinterpret_cast<ObjectT*>(obj->private_data);
    return private_data->template CGetOptionStringLike<>(key, value, length, error);
  }

  template <typename T>
  static AdbcStatusCode CGetOptionInt(T* obj, const char* key, int64_t* value,
                                      AdbcError* error) {
    using ObjectT = ResolveObjectT<DatabaseT, ConnectionT, StatementT, T>;
    auto private_data = reinterpret_cast<ObjectT*>(obj->private_data);
    return private_data->template CGetOptionNumeric<>(key, value, error);
  }

  template <typename T>
  static AdbcStatusCode CGetOptionDouble(T* obj, const char* key, double* value,
                                         AdbcError* error) {
    using ObjectT = ResolveObjectT<DatabaseT, ConnectionT, StatementT, T>;
    auto private_data = reinterpret_cast<ObjectT*>(obj->private_data);
    return private_data->template CGetOptionNumeric<>(key, value, error);
  }
  // TODO: all trampolines need to check for database

  // Database trampolines
  static AdbcStatusCode CDatabaseInit(AdbcDatabase* database, AdbcError* error) {
    auto private_data = reinterpret_cast<DatabaseT*>(database->private_data);
    return private_data->Init(nullptr, error);
  }

  // Connection trampolines
  static AdbcStatusCode CConnectionInit(AdbcConnection* connection,
                                        AdbcDatabase* database, AdbcError* error) {
    auto private_data = reinterpret_cast<ConnectionT*>(connection->private_data);
    return private_data->Init(database->private_data, error);
  }

  static AdbcStatusCode CConnectionCancel(AdbcConnection* connection, AdbcError* error) {
    auto private_data = reinterpret_cast<ConnectionT*>(connection->private_data);
    return private_data->Cancel(error);
  }

  static AdbcStatusCode CConnectionGetInfo(AdbcConnection* connection,
                                           const uint32_t* info_codes,
                                           size_t info_codes_length,
                                           ArrowArrayStream* out, AdbcError* error) {
    auto private_data = reinterpret_cast<ConnectionT*>(connection->private_data);
    return private_data->GetInfo(info_codes, info_codes_length, out, error);
  }

  static AdbcStatusCode CConnectionGetObjects(AdbcConnection* connection, int depth,
                                              const char* catalog, const char* db_schema,
                                              const char* table_name,
                                              const char** table_type,
                                              const char* column_name,
                                              ArrowArrayStream* out, AdbcError* error) {
    auto private_data = reinterpret_cast<ConnectionT*>(connection->private_data);
    return private_data->GetObjects(depth, catalog, db_schema, table_name, table_type,
                                    column_name, out, error);
  }

  static AdbcStatusCode CConnectionGetStatistics(
      AdbcConnection* connection, const char* catalog, const char* db_schema,
      const char* table_name, char approximate, ArrowArrayStream* out, AdbcError* error) {
    auto private_data = reinterpret_cast<ConnectionT*>(connection->private_data);
    return private_data->GetStatistics(catalog, db_schema, table_name, approximate, out,
                                       error);
  }

  static AdbcStatusCode CConnectionGetStatisticNames(AdbcConnection* connection,
                                                     ArrowArrayStream* out,
                                                     AdbcError* error) {
    auto private_data = reinterpret_cast<ConnectionT*>(connection->private_data);
    return private_data->GetStatisticNames(out, error);
  }

  static AdbcStatusCode CConnectionGetTableSchema(AdbcConnection* connection,
                                                  const char* catalog,
                                                  const char* db_schema,
                                                  const char* table_name,
                                                  ArrowSchema* schema, AdbcError* error) {
    auto private_data = reinterpret_cast<ConnectionT*>(connection->private_data);
    return private_data->GetTableSchema(catalog, db_schema, table_name, schema, error);
  }

  static AdbcStatusCode CConnectionGetTableTypes(AdbcConnection* connection,
                                                 ArrowArrayStream* out,
                                                 AdbcError* error) {
    auto private_data = reinterpret_cast<ConnectionT*>(connection->private_data);
    return private_data->GetTableTypes(out, error);
  }

  static AdbcStatusCode CConnectionReadPartition(AdbcConnection* connection,
                                                 const uint8_t* serialized_partition,
                                                 size_t serialized_length,
                                                 ArrowArrayStream* out,
                                                 AdbcError* error) {
    auto private_data = reinterpret_cast<ConnectionT*>(connection->private_data);
    return private_data->ReadPartition(serialized_partition, serialized_length, out,
                                       error);
  }

  static AdbcStatusCode CConnectionCommit(AdbcConnection* connection, AdbcError* error) {
    auto private_data = reinterpret_cast<ConnectionT*>(connection->private_data);
    return private_data->Commit(error);
  }

  static AdbcStatusCode CConnectionRollback(AdbcConnection* connection,
                                            AdbcError* error) {
    auto private_data = reinterpret_cast<ConnectionT*>(connection->private_data);
    return private_data->Rollback(error);
  }

  // Statement trampolines
  static AdbcStatusCode CStatementNew(AdbcConnection* connection,
                                      AdbcStatement* statement, AdbcError* error) {
    auto private_data = new StatementT();
    AdbcStatusCode status = private_data->Init(connection->private_data, error);
    if (status != ADBC_STATUS_OK) {
      delete private_data;
    }

    statement->private_data = private_data;
    return ADBC_STATUS_OK;
  }

  static AdbcStatusCode CStatementExecutePartitions(AdbcStatement* statement,
                                                    struct ArrowSchema* schema,
                                                    struct AdbcPartitions* partitions,
                                                    int64_t* rows_affected,
                                                    AdbcError* error) {
    auto private_data = reinterpret_cast<StatementT*>(statement->private_data);
    return private_data->ExecutePartitions(schema, partitions, rows_affected, error);
  }

  static AdbcStatusCode CStatementExecuteQuery(AdbcStatement* statement,
                                               ArrowArrayStream* stream,
                                               int64_t* rows_affected, AdbcError* error) {
    auto private_data = reinterpret_cast<StatementT*>(statement->private_data);
    return private_data->ExecuteQuery(stream, rows_affected, error);
  }

  static AdbcStatusCode CStatementExecuteSchema(AdbcStatement* statement,
                                                ArrowSchema* schema, AdbcError* error) {
    auto private_data = reinterpret_cast<StatementT*>(statement->private_data);
    return private_data->ExecuteSchema(schema, error);
  }

  static AdbcStatusCode CStatementGetParameterSchema(AdbcStatement* statement,
                                                     ArrowSchema* schema,
                                                     AdbcError* error) {
    auto private_data = reinterpret_cast<StatementT*>(statement->private_data);
    return private_data->GetParameterSchema(schema, error);
  }

  static AdbcStatusCode CStatementPrepare(AdbcStatement* statement, AdbcError* error) {
    auto private_data = reinterpret_cast<StatementT*>(statement->private_data);
    return private_data->Prepare(error);
  }

  static AdbcStatusCode CStatementSetSqlQuery(AdbcStatement* statement, const char* query,
                                              AdbcError* error) {
    auto private_data = reinterpret_cast<StatementT*>(statement->private_data);
    return private_data->SetSqlQuery(query, error);
  }

  static AdbcStatusCode CStatementSetSubstraitPlan(AdbcStatement* statement,
                                                   const uint8_t* plan, size_t length,
                                                   AdbcError* error) {
    auto private_data = reinterpret_cast<StatementT*>(statement->private_data);
    return private_data->SetSubstraitPlan(plan, length, error);
  }

  static AdbcStatusCode CStatementBind(AdbcStatement* statement, ArrowArray* values,
                                       ArrowSchema* schema, AdbcError* error) {
    auto private_data = reinterpret_cast<StatementT*>(statement->private_data);
    return private_data->Bind(values, schema, error);
  }

  static AdbcStatusCode CStatementBindStream(AdbcStatement* statement,
                                             ArrowArrayStream* stream, AdbcError* error) {
    auto private_data = reinterpret_cast<StatementT*>(statement->private_data);
    return private_data->BindStream(stream, error);
  }

  static AdbcStatusCode CStatementCancel(AdbcStatement* statement, AdbcError* error) {
    auto private_data = reinterpret_cast<StatementT*>(statement->private_data);
    return private_data->Cancel(error);
  }
};

}  // namespace adbc::driver

template <>
struct fmt::formatter<adbc::driver::Option> : fmt::nested_formatter<std::string_view> {
  auto format(const adbc::driver::Option& option, fmt::format_context& ctx) {
    return write_padded(ctx, [=](auto out) {
      return std::visit(
          [&](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, adbc::driver::Option::NotFound>) {
              return fmt::format_to(out, "(missing option)");
            } else if constexpr (std::is_same_v<T, std::optional<std::string>>) {
              if (value) {
                return fmt::format_to(out, "'{}'", *value);
              } else {
                return fmt::format_to(out, "(NULL)");
              }
            } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
              return fmt::format_to(out, "({} bytes)", value.size());
            } else {
              return fmt::format_to(out, "{}", value);
            }
          },
          option.value());
    });
  }
};
