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

window.addEventListener("DOMContentLoaded", () => {
    // Injected by template
    window.fetch(versionListingPath)
        .then((r) => r.text())
        .then((text) => {
            const root = document.querySelector("#version-switcher ul");
            const versions = text
                  .trim()
                  .split(/\n/g)
                  .map((version) => {
                      return version.split(/;/);
                  });
            const versionRegex = /^([0-9]+)\.([0-9]+)\.([0-9]+)(.*)$/;
            versions.sort((x, y) => {
                const lhs = x[1].match(versionRegex);
                const rhs = y[1].match(versionRegex);
                if (lhs === null && rhs === null) {
                    return x[1].localeCompare(y[1]);
                }
                if (lhs === null) return 1;
                if (rhs === null) return -1;
                // Major
                const lhsMajor = parseInt(lhs[1], 10);
                const rhsMajor = parseInt(rhs[1], 10);
                if (lhsMajor < rhsMajor) {
                    return -1;
                } else if (lhsMajor > rhsMajor) {
                    return 1;
                }

                const lhsMinor = parseInt(lhs[2], 10);
                const rhsMinor = parseInt(rhs[2], 10);
                if (lhsMinor < rhsMinor) {
                    return -1;
                } else if (lhsMinor > rhsMinor) {
                    return 1;
                }

                const lhsPatch = parseInt(lhs[3], 10);
                const rhsPatch = parseInt(rhs[3], 10);
                if (lhsPatch < rhsPatch) {
                    return -1;
                } else if (lhsPatch > rhsPatch) {
                    return 1;
                }

                return lhs[4].localeCompare(rhs[4]);
            });
            versions.forEach((version) => {
                const el = document.createElement("a");
                el.setAttribute("href", versionsRoot + "/" + version[0]);
                el.innerText = version[1];
                if (version[1] === currentVersion) {
                    el.classList.toggle("active");
                }
                const li = document.createElement("li");
                li.appendChild(el);
                root.appendChild(li);
            });
        });
});
