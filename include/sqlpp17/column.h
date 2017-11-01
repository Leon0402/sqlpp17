#pragma once

/*
Copyright (c) 2016, Roland Bock
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

#include <sqlpp17/alias.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename TableAlias, typename ColumnSpec>
  class column_t
  {
  public:
    using _alias_t = typename ColumnSpec::_alias_t;

    template <typename Alias>
    constexpr auto as(const Alias&) const
    {
      return alias_t<column_t, Alias>{{}};
    }
  };

  template <typename Table, typename Spec>
  struct value_type_of<column_t<Table, Spec>>
  {
    using type = typename Spec::value_type;
  };

  template <typename Table, typename Spec>
  constexpr auto is_expression_v<column_t<Table, Spec>> = true;

  template <typename Table, typename Spec>
  constexpr auto is_selectable_v<column_t<Table, Spec>> = true;

  template <typename Table, typename Spec>
  constexpr auto must_not_insert_v<column_t<Table, Spec>> = !!(Spec::tags & tag::must_not_insert);

  template <typename Table, typename Spec>
  constexpr auto must_not_update_v<column_t<Table, Spec>> = !!(Spec::tags & tag::must_not_update);

  template <typename Table, typename Spec>
  constexpr auto has_default_v<column_t<Table, Spec>> = !!(Spec::tags & tag::has_default);

  template <typename Table, typename Spec>
  constexpr auto is_insert_required_v<column_t<Table, Spec>> =
      !(Spec::tags & tag::has_default) && !(Spec::tags & tag::must_not_insert);

  template <typename Context, typename Table, typename ColumnSpec>
  decltype(auto) operator<<(Context& context, const column_t<Table, ColumnSpec>& t)
  {
    return context << name_of_v<Table> << '.' << name_of_v<ColumnSpec>;
  }

}  // namespace sqlpp
