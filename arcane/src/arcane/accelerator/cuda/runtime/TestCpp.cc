﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* TestCpp.cc                                                  (C) 2000-2022 */
/*                                                                           */
/* Fichier de tests pour CUDA.                                               */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/utils/ArcaneGlobal.h"

extern "C"
int arcaneTestCuda1();
extern "C"
int arcaneTestCuda2();
extern "C"
int arcaneTestCuda3();
extern "C"
int arcaneTestCudaNumArray();
extern "C"
int arcaneTestCudaReduction();

extern "C" ARCANE_EXPORT
int func0()
{
  arcaneTestCuda3();
  arcaneTestCudaNumArray();
  arcaneTestCudaReduction();
  return 0;
}
