﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "arcane/tests/AdiProjection_axl.h"

#include "arcane/MeshAreaAccessor.h"
#include "arcane/MeshArea.h"
#include "arcane/ISubDomain.h"

#include "arcane/IMesh.h"

#include "arcane/cea/ICartesianMesh.h"
#include "arcane/cea/CellDirectionMng.h"
#include "arcane/cea/FaceDirectionMng.h"
#include "arcane/cea/NodeDirectionMng.h"

namespace Arcane
{

class AdiProjectionModule
: public ArcaneAdiProjectionObject
{ 

public:

  explicit AdiProjectionModule(const ModuleBuildInfo& mb);
  ~AdiProjectionModule();
  
public:
  
  virtual VersionInfo versionInfo() const { return VersionInfo(1,1,1); }

public:
  
  void copyEulerianCoordinates();
  void cartesianHydroMain();
  virtual void cartesianHydroStartInit();

 public:
  
  static void staticInitialize(ISubDomain* sd);

 private:

  ICartesianMesh* m_cartesian_mesh = nullptr;

 private:

  void evolvePrimalUpwindedVariables(Integer direction);
  void evolveDualUpwindedVariables(Integer direction);
  void computePressure();
  void computePressureGradient(Integer direction);
  void checkNodalMassConservation();
  void copyCurrentVariablesToOldVariables();

  void computePrimalMassFluxInner(Integer direction);
  void computePrimalMassFluxBoundary(Integer direction);

  void computeDualMassFluxInner(Integer direction);
  void computeDualMassFluxBoundary(Integer direction);
  void prepareLagrangianVariables();
  void checkLagrangianVariablesConsistency();

  void _evolveDualUpwindedVariables1();
  void _evolvePrimalUpwindedVariablesV2(Integer direction);
};

} // End namespace Arcane
