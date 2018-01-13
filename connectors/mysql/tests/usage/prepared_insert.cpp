/*
Copyright (c) 2017, Roland Bock
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this
   list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <iostream>

#include <sqlpp17/clause/create_table.h>
#include <sqlpp17/clause/drop_table.h>
#include <sqlpp17/clause/insert_into.h>
#include <sqlpp17/parameter.h>

#include <sqlpp17/mysql/connection.h>

#include <tables/TabDepartment.h>
#include <tables/TabPerson.h>

using test::tabDepartment;
using test::tabPerson;

SQLPP_CREATE_NAME_TAG(pName);
SQLPP_CREATE_NAME_TAG(pIsManager);
SQLPP_CREATE_NAME_TAG(pAddress);

auto print_debug(std::string_view message)
{
  std::cout << "Debug: " << message << std::endl;
}

namespace mysql = sqlpp::mysql;
int main()
{
  mysql::global_library_init();

  auto config = mysql::connection_config_t{};
  config.user = "root";
#warning : This needs to be configurable
  config.password = "test";
  config.database = "sqlpp_mysql";
  config.debug = print_debug;
  try
  {
    auto db = mysql::connection_t{config};
  }
  catch (const sqlpp::exception& e)
  {
    std::cerr << "For testing, you'll need to create a database sqlpp_mysql for user root (no password)" << std::endl;
    std::cerr << e.what() << std::endl;
    return 1;
  }
  try
  {
    auto db = mysql::connection_t{config};
    db(drop_table(tabDepartment));
    db(drop_table(tabPerson));
    db(create_table(tabDepartment));
    db(create_table(tabPerson));

    {
      auto prepared_insert = db.prepare(insert_into(tabDepartment).default_values());
      [[maybe_unused]] const auto id = execute(prepared_insert);
    }

    {
      auto s = db.prepare(insert_into(tabPerson).set(tabPerson.isManager = true,
                                                     tabPerson.name = ::sqlpp::parameter<std::string>(pName)));
      s.pName = "Herb";
      [[maybe_unused]] const auto id = execute(s);
    }

    {
      auto s = db.prepare(insert_into(tabPerson).set(
          tabPerson.isManager = ::sqlpp::parameter<bool>(pIsManager),
          tabPerson.name = ::sqlpp::parameter<std::string>(pName),
          tabPerson.address = ::sqlpp::parameter<::std::optional<std::string>>(pAddress), tabPerson.language = "C++"));
      s.pIsManager = true;
      s.pName = "Herb";
      s.pAddress = "Somewhere";
      s.pAddress = std::nullopt;
      s.pAddress.reset();

      [[maybe_unused]] const auto id = execute(s);
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
}

