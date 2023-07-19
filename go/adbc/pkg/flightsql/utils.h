// Code generated by _tmpl/utils.h.tmpl. DO NOT EDIT.

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

// clang-format off
//go:build driverlib
//  clang-format on

#pragma once

#include <stdlib.h>
#include "../../drivermgr/adbc.h"

AdbcStatusCode FlightSQLDatabaseGetOption(struct AdbcDatabase*, const char*, char*,
                                          size_t*, struct AdbcError*);
AdbcStatusCode FlightSQLDatabaseGetOptionBytes(struct AdbcDatabase*, const char*,
                                               uint8_t*, size_t*, struct AdbcError*);
AdbcStatusCode FlightSQLDatabaseGetOptionDouble(struct AdbcDatabase*, const char*,
                                                double*, struct AdbcError*);
AdbcStatusCode FlightSQLDatabaseGetOptionInt(struct AdbcDatabase*, const char*, int64_t*,
                                             struct AdbcError*);
AdbcStatusCode FlightSQLDatabaseInit(struct AdbcDatabase* db, struct AdbcError* err);
AdbcStatusCode FlightSQLDatabaseNew(struct AdbcDatabase* db, struct AdbcError* err);
AdbcStatusCode FlightSQLDatabaseRelease(struct AdbcDatabase* db, struct AdbcError* err);
AdbcStatusCode FlightSQLDatabaseSetOption(struct AdbcDatabase* db, const char* key,
                                          const char* value, struct AdbcError* err);
AdbcStatusCode FlightSQLDatabaseSetOptionBytes(struct AdbcDatabase*, const char*,
                                               const uint8_t*, size_t, struct AdbcError*);
AdbcStatusCode FlightSQLDatabaseSetOptionDouble(struct AdbcDatabase*, const char*, double,
                                                struct AdbcError*);
AdbcStatusCode FlightSQLDatabaseSetOptionInt(struct AdbcDatabase*, const char*, int64_t,
                                             struct AdbcError*);

AdbcStatusCode FlightSQLConnectionCancel(struct AdbcConnection*, struct AdbcError*);
AdbcStatusCode FlightSQLConnectionCommit(struct AdbcConnection* cnxn,
                                         struct AdbcError* err);
AdbcStatusCode FlightSQLConnectionGetInfo(struct AdbcConnection* cnxn, uint32_t* codes,
                                          size_t len, struct ArrowArrayStream* out,
                                          struct AdbcError* err);
AdbcStatusCode FlightSQLConnectionGetObjects(
    struct AdbcConnection* cnxn, int depth, const char* catalog, const char* dbSchema,
    const char* tableName, const char** tableType, const char* columnName,
    struct ArrowArrayStream* out, struct AdbcError* err);
AdbcStatusCode FlightSQLConnectionGetOption(struct AdbcConnection*, const char*, char*,
                                            size_t*, struct AdbcError*);
AdbcStatusCode FlightSQLConnectionGetOptionBytes(struct AdbcConnection*, const char*,
                                                 uint8_t*, size_t*, struct AdbcError*);
AdbcStatusCode FlightSQLConnectionGetOptionDouble(struct AdbcConnection*, const char*,
                                                  double*, struct AdbcError*);
AdbcStatusCode FlightSQLConnectionGetOptionInt(struct AdbcConnection*, const char*,
                                               int64_t*, struct AdbcError*);
AdbcStatusCode FlightSQLConnectionGetStatistics(struct AdbcConnection*, const char*,
                                                const char*, const char*, char,
                                                struct ArrowArrayStream*,
                                                struct AdbcError*);
AdbcStatusCode FlightSQLConnectionGetStatisticNames(struct AdbcConnection*,
                                                    struct ArrowArrayStream*,
                                                    struct AdbcError*);
AdbcStatusCode FlightSQLConnectionGetTableSchema(
    struct AdbcConnection* cnxn, const char* catalog, const char* dbSchema,
    const char* tableName, struct ArrowSchema* schema, struct AdbcError* err);
AdbcStatusCode FlightSQLConnectionGetTableTypes(struct AdbcConnection* cnxn,
                                                struct ArrowArrayStream* out,
                                                struct AdbcError* err);
AdbcStatusCode FlightSQLConnectionInit(struct AdbcConnection* cnxn,
                                       struct AdbcDatabase* db, struct AdbcError* err);
AdbcStatusCode FlightSQLConnectionNew(struct AdbcConnection* cnxn, struct AdbcError* err);
AdbcStatusCode FlightSQLConnectionReadPartition(struct AdbcConnection* cnxn,
                                                const uint8_t* serialized,
                                                size_t serializedLen,
                                                struct ArrowArrayStream* out,
                                                struct AdbcError* err);
AdbcStatusCode FlightSQLConnectionRelease(struct AdbcConnection* cnxn,
                                          struct AdbcError* err);
AdbcStatusCode FlightSQLConnectionRollback(struct AdbcConnection* cnxn,
                                           struct AdbcError* err);
AdbcStatusCode FlightSQLConnectionSetOption(struct AdbcConnection* cnxn, const char* key,
                                            const char* val, struct AdbcError* err);
AdbcStatusCode FlightSQLConnectionSetOptionBytes(struct AdbcConnection*, const char*,
                                                 const uint8_t*, size_t,
                                                 struct AdbcError*);
AdbcStatusCode FlightSQLConnectionSetOptionDouble(struct AdbcConnection*, const char*,
                                                  double, struct AdbcError*);
AdbcStatusCode FlightSQLConnectionSetOptionInt(struct AdbcConnection*, const char*,
                                               int64_t, struct AdbcError*);

AdbcStatusCode FlightSQLStatementBind(struct AdbcStatement* stmt,
                                      struct ArrowArray* values,
                                      struct ArrowSchema* schema, struct AdbcError* err);
AdbcStatusCode FlightSQLStatementBindStream(struct AdbcStatement* stmt,
                                            struct ArrowArrayStream* stream,
                                            struct AdbcError* err);
AdbcStatusCode FlightSQLStatementCancel(struct AdbcStatement*, struct AdbcError*);
AdbcStatusCode FlightSQLStatementExecuteQuery(struct AdbcStatement* stmt,
                                              struct ArrowArrayStream* out,
                                              int64_t* affected, struct AdbcError* err);
AdbcStatusCode FlightSQLStatementExecutePartitions(struct AdbcStatement* stmt,
                                                   struct ArrowSchema* schema,
                                                   struct AdbcPartitions* partitions,
                                                   int64_t* affected,
                                                   struct AdbcError* err);
AdbcStatusCode FlightSQLStatementExecuteSchema(struct AdbcStatement*, struct ArrowSchema*,
                                               struct AdbcError*);
AdbcStatusCode FlightSQLStatementGetOption(struct AdbcStatement*, const char*, char*,
                                           size_t*, struct AdbcError*);
AdbcStatusCode FlightSQLStatementGetOptionBytes(struct AdbcStatement*, const char*,
                                                uint8_t*, size_t*, struct AdbcError*);
AdbcStatusCode FlightSQLStatementGetOptionDouble(struct AdbcStatement*, const char*,
                                                 double*, struct AdbcError*);
AdbcStatusCode FlightSQLStatementGetOptionInt(struct AdbcStatement*, const char*,
                                              int64_t*, struct AdbcError*);
AdbcStatusCode FlightSQLStatementGetParameterSchema(struct AdbcStatement* stmt,
                                                    struct ArrowSchema* schema,
                                                    struct AdbcError* err);
AdbcStatusCode FlightSQLStatementNew(struct AdbcConnection* cnxn,
                                     struct AdbcStatement* stmt, struct AdbcError* err);
AdbcStatusCode FlightSQLStatementPrepare(struct AdbcStatement* stmt,
                                         struct AdbcError* err);
AdbcStatusCode FlightSQLStatementRelease(struct AdbcStatement* stmt,
                                         struct AdbcError* err);
AdbcStatusCode FlightSQLStatementSetOption(struct AdbcStatement* stmt, const char* key,
                                           const char* value, struct AdbcError* err);
AdbcStatusCode FlightSQLStatementSetOptionBytes(struct AdbcStatement*, const char*,
                                                const uint8_t*, size_t,
                                                struct AdbcError*);
AdbcStatusCode FlightSQLStatementSetOptionDouble(struct AdbcStatement*, const char*,
                                                 double, struct AdbcError*);
AdbcStatusCode FlightSQLStatementSetOptionInt(struct AdbcStatement*, const char*, int64_t,
                                              struct AdbcError*);
AdbcStatusCode FlightSQLStatementSetSqlQuery(struct AdbcStatement* stmt,
                                             const char* query, struct AdbcError* err);
AdbcStatusCode FlightSQLStatementSetSubstraitPlan(struct AdbcStatement* stmt,
                                                  const uint8_t* plan, size_t length,
                                                  struct AdbcError* err);

AdbcStatusCode FlightSQLDriverInit(int version, void* rawDriver, struct AdbcError* err);

static inline void FlightSQLerrRelease(struct AdbcError* error) { error->release(error); }

void FlightSQL_release_error(struct AdbcError* error);
