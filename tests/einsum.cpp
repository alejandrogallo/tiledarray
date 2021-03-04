/*
* This file is a part of TiledArray.
* Copyright (C) 2013  Virginia Tech
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include "tiledarray.h"
#include "TiledArray/expressions/einsum.h"
#include "unit_test_config.h"

BOOST_AUTO_TEST_SUITE(einsum)

using namespace TiledArray;
using TiledArray::expressions::einsum;

BOOST_AUTO_TEST_CASE(ij_ik_jl) {
  TArrayD a,b,c;
  c = einsum(a("i,k"), b("k,j"));
  c = einsum(a("i,j"), b("i,j"), "i,j");
  c = einsum(a("i,k"), b("i,k"), "i");
  c = einsum(a("i,k"), b("j,k"), "i,j,k");
}


BOOST_AUTO_TEST_SUITE_END()
