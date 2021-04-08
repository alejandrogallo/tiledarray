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
  auto &world = get_default_world();
  TiledRange1 tr1{ 0, 3, 8 };
  TArrayD a(world, TiledRange{ tr1, tr1, tr1 });
  TArrayD b(world, TiledRange{ tr1, tr1, tr1 });
  a.fill(1);
  b.fill(1);
  TArrayD c;
  c = einsum(a("h,i,k"), b("h,k,j"));
  world.gop.fence();
  c = einsum(a("h,i,k"), b("h,k,j"), "i,j");
  world.gop.fence();
  c = einsum(a("i,h,k"), b("k,j,h"), "i,k,j");
  world.gop.fence();
  //std::cout << c << std::endl;
}


BOOST_AUTO_TEST_SUITE_END()
