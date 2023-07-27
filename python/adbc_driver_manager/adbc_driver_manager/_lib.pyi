# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

# NOTE: generated with mypy's stubgen, then hand-edited to fix things

from typing import Any, ClassVar, Dict, List, Literal, Optional, Tuple, Union

from typing import overload
import enum
import typing

INGEST_OPTION_MODE: str
INGEST_OPTION_MODE_APPEND: str
INGEST_OPTION_MODE_CREATE: str
INGEST_OPTION_MODE_CREATE_APPEND: str
INGEST_OPTION_MODE_REPLACE: str
INGEST_OPTION_TARGET_TABLE: str

class AdbcConnection(_AdbcHandle):
    def __init__(self, database: "AdbcDatabase", **kwargs: str) -> None: ...
    def cancel(self) -> None: ...
    def close(self) -> None: ...
    def commit(self) -> None: ...
    def get_info(
        self, info_codes: Optional[List[Union[int, "AdbcInfoCode"]]] = None
    ) -> "ArrowArrayStreamHandle": ...
    def get_option(self, key: str) -> str: ...
    def get_option_bytes(self, key: str) -> bytes: ...
    def get_option_float(self, key: str) -> float: ...
    def get_option_int(self, key: str) -> int: ...
    def get_objects(
        self,
        depth: "GetObjectsDepth",
        catalog: Optional[str] = None,
        db_schema: Optional[str] = None,
        table_name: Optional[str] = None,
        table_types: Optional[List[str]] = None,
        column_name: Optional[str] = None,
    ) -> "ArrowArrayStreamHandle": ...
    def get_table_schema(
        self,
        catalog: Optional[str],
        db_schema: Optional[str],
        table_name: str,
    ) -> "ArrowSchemaHandle": ...
    def get_table_types(self) -> "ArrowArrayStreamHandle": ...
    def read_partition(self, partition: bytes) -> "ArrowArrayStreamHandle": ...
    def rollback(self) -> None: ...
    def set_autocommit(self, enabled: bool) -> None: ...
    def set_options(self, **kwargs: Union[bytes, float, int, str]) -> None: ...

class AdbcDatabase(_AdbcHandle):
    def __init__(self, **kwargs: str) -> None: ...
    def close(self) -> None: ...
    def get_option(self, key: str) -> str: ...
    def get_option_bytes(self, key: str) -> bytes: ...
    def get_option_float(self, key: str) -> float: ...
    def get_option_int(self, key: str) -> int: ...
    def set_options(self, **kwargs: Union[bytes, float, int, str]) -> None: ...

class AdbcInfoCode(enum.IntEnum):
    DRIVER_ARROW_VERSION = ...
    DRIVER_NAME = ...
    DRIVER_VERSION = ...
    VENDOR_ARROW_VERSION = ...
    VENDOR_NAME = ...
    VENDOR_VERSION = ...

class AdbcStatement(_AdbcHandle):
    def __init__(self, *args, **kwargs) -> None: ...
    def bind(self, *args, **kwargs) -> Any: ...
    def bind_stream(self, *args, **kwargs) -> Any: ...
    def cancel(self) -> None: ...
    def close(self) -> None: ...
    def execute_partitions(self, *args, **kwargs) -> Any: ...
    def execute_query(self, *args, **kwargs) -> Any: ...
    def execute_schema(self) -> "ArrowSchemaHandle": ...
    def execute_update(self, *args, **kwargs) -> Any: ...
    def get_option(self, key: str) -> str: ...
    def get_option_bytes(self, key: str) -> bytes: ...
    def get_option_float(self, key: str) -> float: ...
    def get_option_int(self, key: str) -> int: ...
    def get_parameter_schema(self, *args, **kwargs) -> Any: ...
    def prepare(self, *args, **kwargs) -> Any: ...
    def set_options(self, **kwargs: Union[bytes, float, int, str]) -> None: ...
    def set_sql_query(self, *args, **kwargs) -> Any: ...
    def set_substrait_plan(self, *args, **kwargs) -> Any: ...
    def __reduce__(self) -> Any: ...
    def __setstate__(self, state) -> Any: ...

class AdbcStatusCode(enum.IntEnum):
    ALREADY_EXISTS = ...
    CANCELLED = ...
    INTEGRITY = ...
    INTERNAL = ...
    INVALID_ARGUMENT = ...
    INVALID_DATA = ...
    INVALID_STATE = ...
    IO = ...
    NOT_FOUND = ...
    NOT_IMPLEMENTED = ...
    OK = ...
    TIMEOUT = ...
    UNAUTHENTICATED = ...
    UNAUTHORIZED = ...
    UNKNOWN = ...

class ArrowArrayHandle:
    address: Any

class ArrowArrayStreamHandle:
    address: Any

class ArrowSchemaHandle:
    address: Any

class DataError(DatabaseError): ...
class DatabaseError(Error): ...

class Error(Exception):
    status_code: AdbcStatusCode
    vendor_code: Optional[int]
    sqlstate: Optional[str]

    def __init__(
        self,
        message: str,
        *,
        status_code: Union[int, AdbcStatusCode],
        vendor_code: Optional[str] = None,
        sqlstate: Optional[str] = None
    ) -> None: ...

class GetObjectsDepth(enum.IntEnum):
    ALL = ...
    CATALOGS = ...
    COLUMNS = ...
    DB_SCHEMAS = ...
    TABLES = ...

class IntegrityError(DatabaseError): ...
class InterfaceError(Error): ...
class InternalError(DatabaseError): ...

class NotSupportedError(DatabaseError):
    def __init__(
        self,
        message: str,
        *,
        vendor_code: Optional[str] = None,
        sqlstate: Optional[str] = None
    ) -> None: ...

class OperationalError(DatabaseError): ...
class ProgrammingError(DatabaseError): ...
class Warning(UserWarning): ...

class _AdbcHandle:
    def __init__(self, *args, **kwargs) -> None: ...
    def __enter__(self) -> Any: ...
    def __exit__(self, type, value, traceback) -> Any: ...

def _test_error(
    status_code: Union[int, AdbcStatusCode],
    message: str,
    vendor_code: Optional[int],
    sqlstate: Optional[str],
) -> Error: ...
