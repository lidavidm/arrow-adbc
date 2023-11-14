/*
* Licensed to the Apache Software Foundation (ASF) under one or more
* contributor license agreements.  See the NOTICE file distributed with
* this work for additional information regarding copyright ownership.
* The ASF licenses this file to You under the Apache License, Version 2.0
* (the "License"); you may not use this file except in compliance with
* the License.  You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Common;
using System.Data.SqlTypes;
using System.IO;
using Apache.Arrow.Adbc.Client;
using Apache.Arrow.Adbc.Tests.Xunit;
using Xunit;

namespace Apache.Arrow.Adbc.Tests.Drivers.Interop.Snowflake
{
    /// <summary>
    /// Class for testing the ADBC Client using the Snowflake ADBC driver.
    /// </summary>
    /// <remarks>
    /// Tests are ordered to ensure data is created
    /// for the other queries to run.
    /// </remarks>
    [TestCaseOrderer("Apache.Arrow.Adbc.Tests.Xunit.TestOrderer", "Apache.Arrow.Adbc.Tests")]
    public class ClientTests
    {
        public ClientTests()
        {
           Skip.IfNot(Utils.CanExecuteTestConfig(SnowflakeTestingUtils.SNOWFLAKE_TEST_CONFIG_VARIABLE));
        }

        /// <summary>
        /// Validates if the client execute updates.
        /// </summary>
        [SkippableFact, Order(1)]
        public void CanClientExecuteUpdate()
        {
            SnowflakeTestConfiguration testConfiguration = Utils.LoadTestConfiguration<SnowflakeTestConfiguration>(SnowflakeTestingUtils.SNOWFLAKE_TEST_CONFIG_VARIABLE);

            using (Adbc.Client.AdbcConnection adbcConnection = GetSnowflakeAdbcConnection(testConfiguration))
            {
                string[] queries = SnowflakeTestingUtils.GetQueries(testConfiguration);

                List<int> expectedResults = new List<int>() { -1, 1, 1 };

                Tests.ClientTests.CanClientExecuteUpdate(adbcConnection, testConfiguration, queries, expectedResults);
            }
        }

        /// <summary>
        /// Validates if the client execute updates using the reader.
        /// </summary>
        [SkippableFact, Order(2)]
        public void CanClientExecuteUpdateUsingExecuteReader()
        {
            SnowflakeTestConfiguration testConfiguration = Utils.LoadTestConfiguration<SnowflakeTestConfiguration>(SnowflakeTestingUtils.SNOWFLAKE_TEST_CONFIG_VARIABLE);

            using (Adbc.Client.AdbcConnection adbcConnection = GetSnowflakeAdbcConnection(testConfiguration))
            {
                adbcConnection.Open();

                string[] queries = SnowflakeTestingUtils.GetQueries(testConfiguration);

                List<object> expectedResults = new List<object>() { $"Table {testConfiguration.Metadata.Table} successfully created.", new SqlDecimal(1L), new SqlDecimal(1L) };

                for (int i = 0; i < queries.Length; i++)
                {
                    string query = queries[i];
                    AdbcCommand adbcCommand = adbcConnection.CreateCommand();
                    adbcCommand.CommandText = query;

                    AdbcDataReader reader = adbcCommand.ExecuteReader(CommandBehavior.Default);

                    if (reader.Read())
                    {
                        Assert.True(expectedResults[i].Equals(reader.GetValue(0)), $"The expected affected rows do not match the actual affected rows at position {i}.");
                    }
                    else
                    {
                        Assert.Fail("Could not read the records");
                    }
                }
            }
        }

        /// <summary>
        /// Validates if the client can get the schema.
        /// </summary>
        [SkippableFact, Order(3)]
        public void CanClientGetSchema()
        {
            SnowflakeTestConfiguration testConfiguration = Utils.LoadTestConfiguration<SnowflakeTestConfiguration>(SnowflakeTestingUtils.SNOWFLAKE_TEST_CONFIG_VARIABLE);

            using (Adbc.Client.AdbcConnection adbcConnection = GetSnowflakeAdbcConnection(testConfiguration))
            {
                Tests.ClientTests.CanClientGetSchema(adbcConnection, testConfiguration);
            }
        }

        /// <summary>
        /// Validates if the client can connect to a live server
        /// and parse the results.
        /// </summary>
        [SkippableFact, Order(4)]
        public void CanClientExecuteQuery()
        {
            SnowflakeTestConfiguration testConfiguration = Utils.LoadTestConfiguration<SnowflakeTestConfiguration>(SnowflakeTestingUtils.SNOWFLAKE_TEST_CONFIG_VARIABLE);

            using (Adbc.Client.AdbcConnection adbcConnection = GetSnowflakeAdbcConnection(testConfiguration))
            {
                Tests.ClientTests.CanClientExecuteQuery(adbcConnection, testConfiguration);
            }
        }

        // <summary>
        /// Validates if the client can connect to a live server
        /// and parse the results.
        /// </summary>
        [SkippableFact, Order(4)]
        public void CanClientExecuteQueryWithNoResults()
        {
            SnowflakeTestConfiguration testConfiguration = Utils.LoadTestConfiguration<SnowflakeTestConfiguration>(SnowflakeTestingUtils.SNOWFLAKE_TEST_CONFIG_VARIABLE);
            testConfiguration.Query = "SELECT * WHERE 0=1";
            testConfiguration.ExpectedResultsCount = 0;

            using (Adbc.Client.AdbcConnection adbcConnection = GetSnowflakeAdbcConnection(testConfiguration))
            {
                Tests.ClientTests.CanClientExecuteQuery(adbcConnection, testConfiguration);
            }
        }

        /// <summary>
        /// Validates if the client can connect to a live server
        /// using a connection string / private key and parse the results.
        /// </summary>
        [SkippableFact, Order(5)]
        public void CanClientExecuteQueryUsingPrivateKey()
        {
            SnowflakeTestConfiguration testConfiguration = Utils.LoadTestConfiguration<SnowflakeTestConfiguration>(SnowflakeTestingUtils.SNOWFLAKE_TEST_CONFIG_VARIABLE);

            using (Adbc.Client.AdbcConnection adbcConnection = GetSnowflakeAdbcConnectionUsingConnectionString(testConfiguration))
            {
                Tests.ClientTests.CanClientExecuteQuery(adbcConnection, testConfiguration);
            }
        }

        /// <summary>
        /// Validates if the client is retrieving and converting values
        /// to the expected types.
        /// </summary>
        [SkippableFact, Order(6)]
        public void VerifyTypesAndValues()
        {
            SnowflakeTestConfiguration testConfiguration = Utils.LoadTestConfiguration<SnowflakeTestConfiguration>(SnowflakeTestingUtils.SNOWFLAKE_TEST_CONFIG_VARIABLE);

            using (Adbc.Client.AdbcConnection adbcConnection = GetSnowflakeAdbcConnection(testConfiguration))
            {
                SampleDataBuilder sampleDataBuilder = SnowflakeData.GetSampleData();

                Tests.ClientTests.VerifyTypesAndValues(adbcConnection, sampleDataBuilder);
            }
        }

        private Adbc.Client.AdbcConnection GetSnowflakeAdbcConnectionUsingConnectionString(SnowflakeTestConfiguration testConfiguration)
        {
            // see https://arrow.apache.org/adbc/0.5.1/driver/snowflake.html

            DbConnectionStringBuilder builder = new DbConnectionStringBuilder(true);

            builder[SnowflakeParameters.ACCOUNT] = testConfiguration.Account;
            builder[SnowflakeParameters.WAREHOUSE] = testConfiguration.Warehouse;
            builder[SnowflakeParameters.HOST] = testConfiguration.Host;
            builder[SnowflakeParameters.DATABASE] = testConfiguration.Database;
            builder[SnowflakeParameters.USERNAME] = testConfiguration.User;

            if (!string.IsNullOrEmpty(testConfiguration.AuthenticationTokenPath))
            {
                builder[SnowflakeParameters.AUTH_TYPE] = testConfiguration.AuthenticationType;

                string privateKey = File.ReadAllText(testConfiguration.AuthenticationTokenPath);

                if (testConfiguration.AuthenticationType.Equals("auth_jwt", StringComparison.OrdinalIgnoreCase))
                {
                    builder[SnowflakeParameters.PKCS8_VALUE] = privateKey;

                    if(!string.IsNullOrEmpty(testConfiguration.Pkcs8Passcode))
                    {
                        builder[SnowflakeParameters.PKCS8_PASS] = testConfiguration.Pkcs8Passcode;
                    }
                }
            }
            else
            {
                builder[SnowflakeParameters.PASSWORD] = testConfiguration.Password;
            }

            AdbcDriver snowflakeDriver = SnowflakeTestingUtils.GetSnowflakeAdbcDriver(testConfiguration);

            return new Adbc.Client.AdbcConnection(builder.ConnectionString)
            {
                AdbcDriver = snowflakeDriver
            };
        }

        private Adbc.Client.AdbcConnection GetSnowflakeAdbcConnection(SnowflakeTestConfiguration testConfiguration)
        {
            Dictionary<string, string> parameters = new Dictionary<string, string>();

            AdbcDriver snowflakeDriver = SnowflakeTestingUtils.GetSnowflakeAdbcDriver(testConfiguration, out parameters);

            Adbc.Client.AdbcConnection adbcConnection = new Adbc.Client.AdbcConnection(
                snowflakeDriver,
                parameters: parameters,
                options: new Dictionary<string, string>()
            );

            return adbcConnection;
        }
    }
}