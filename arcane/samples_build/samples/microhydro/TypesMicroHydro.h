#ifndef TYPESMICROHYDRO_H
#define TYPESMICROHYDRO_H

#include <arcane/ItemGroup.h>
#include "eos/IEquationOfState.h"

struct TypesMicroHydro
{
  enum eBoundaryCondition
  {
    VelocityX, //!< Vitesse X fix�e
    VelocityY, //!< Vitesse Y fix�e
    VelocityZ, //!< Vitesse Z fix�e
    Unknown //!< Type inconnu
  };
};

#endif

