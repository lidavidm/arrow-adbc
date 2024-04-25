#!/usr/bin/env bash
#
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

set -e
set -u
set -o pipefail

SOURCE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source "${SOURCE_DIR}/utils-common.sh"
source "${SOURCE_DIR}/utils-prepare.sh"

main() {
    if [ "$#" -ne 1 ]; then
        echo "Usage: $0 <rc-num>"
        exit 1
    fi
    local -r rc_number="$1"
    local -r tag="apache-arrow-adbc-${RELEASE}-rc${rc_number}"

    header "Publishing release ${RELEASE}"

    gh release edit \
       --verify-tag \
       --repo "${REPOSITORY}" \
       "${tag}" \
       --title="ADBC Libraries ${RELEASE}" \
       --prerelease=false \
       --tag="apache-arrow-adbc-${RELEASE}"
}

main "$@"
