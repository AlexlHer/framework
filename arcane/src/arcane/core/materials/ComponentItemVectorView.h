﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2023 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* ComponentItemVectorView.h                                   (C) 2000-2023 */
/*                                                                           */
/* Vue sur un vecteur sur des entités composants.                            */
/*---------------------------------------------------------------------------*/
#ifndef ARCANE_CORE_MATERIALS_COMPONENTITEMVECTORVIEW_H
#define ARCANE_CORE_MATERIALS_COMPONENTITEMVECTORVIEW_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/utils/TraceAccessor.h"
#include "arcane/utils/ArrayView.h"

#include "arcane/ItemGroup.h"

#include "arcane/core/materials/MatVarIndex.h"
#include "arcane/core/materials/IMeshComponent.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace ArcaneTest
{
class MeshMaterialTesterModule;
}

namespace Arcane::Materials
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Vue sur un vecteur sur les entités d'un composant.
 *
 * Les constructeurs de cette classe sont internes à %Arcane.
 */
class ARCANE_CORE_EXPORT ComponentItemVectorView
{
  friend class ComponentItemVector;
  friend class MatItemVectorView;
  friend class EnvItemVectorView;
  friend ArcaneTest::MeshMaterialTesterModule;
  template<typename ViewType,typename LambdaType>
  friend class LambdaMatItemRangeFunctorT;

 protected:

  //! Construit un vecteur contenant les entités de \a group pour le composant \a component
  ComponentItemVectorView(IMeshComponent* component,
                          ConstArrayView<MatVarIndex> mvi,
                          ConstArrayView<ComponentItemInternal*> mv)
  : m_matvar_indexes_view(mvi), m_items_internal_main_view(mv), m_component(component)
  {
  }

  //! Construit une vue vide pour le composant \a component
  ComponentItemVectorView(IMeshComponent* component)
  : m_component(component)
  {
  }

  //! Construit une vue à partir d'une autre vue.
  ComponentItemVectorView(IMeshComponent* component,ComponentItemVectorView rhs_view)
  : m_matvar_indexes_view(rhs_view.m_matvar_indexes_view)
  , m_items_internal_main_view(rhs_view.m_items_internal_main_view)
  , m_component(component)
  {
  }

 public:

  //! Nombre d'entités dans la vue
  Integer nbItem() const { return m_matvar_indexes_view.size(); }

  //! Composant associé
  IMeshComponent* component() const { return m_component; }

 public:

  //! Interne à Arcane
  //@{
  ConstArrayView<ComponentItemInternal*> itemsInternalView() const
  { return m_items_internal_main_view; }

  // Tableau des MatVarIndex de cette vue.
  ConstArrayView<MatVarIndex> matvarIndexes() const { return m_matvar_indexes_view; }
  //@}

 private:

  /*!
   * \internal
   * \brief Créé une sous-vue de cette vue.
   * 
   * Cette méthode est interne à Arcane et ne doit pas être utilisée.
   */
  ComponentItemVectorView _subView(Integer begin,Integer size);

 private:

  ConstArrayView<MatVarIndex> m_matvar_indexes_view;
  ConstArrayView<ComponentItemInternal*> m_items_internal_main_view;
  IMeshComponent* m_component;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Vue sur un vecteur sur les entités d'un matériau.
 *
 * Les constructeurs de cette classe sont internes à %Arcane.
 */
class ARCANE_CORE_EXPORT MatItemVectorView
: public ComponentItemVectorView
{
  friend class MatCellVector;
  friend class MeshMaterial;
  template<typename ViewType,typename LambdaType>
  friend class LambdaMatItemRangeFunctorT;

 private:

  //! Construit un vecteur contenant les entités de \a group pour le composant \a component
  MatItemVectorView(IMeshComponent* component,
                    ConstArrayView<MatVarIndex> mv_indexes, 
                    ConstArrayView<ComponentItemInternal*> mv)
  : ComponentItemVectorView(component,mv_indexes,mv){}

  MatItemVectorView(IMeshComponent* component,ComponentItemVectorView v)
  : ComponentItemVectorView(component,v){}

 private:

  /*!
   * \internal
   * \brief Créé une sous-vue de cette vue.
   * 
   * Cette méthode est interne à Arcane et ne doit pas être utilisée.
   */
  MatItemVectorView _subView(Integer begin,Integer size);

 public:

  //! Matériau associé
  IMeshMaterial* material() const;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Vue sur un vecteur sur les entités d'un milieu.
 *
 * Les constructeurs de cette classe sont internes à %Arcane.
 */
class ARCANE_CORE_EXPORT EnvItemVectorView
: public ComponentItemVectorView
{
  friend class EnvCellVector;
  friend class MeshEnvironment;
  template<typename ViewType,typename LambdaType>
  friend class LambdaMatItemRangeFunctorT;

 private:

  //! Construit un vecteur contenant les entités de \a group pour le composant \a component
  EnvItemVectorView(IMeshComponent* component,
                    ConstArrayView<MatVarIndex> mv_indexes,
                    ConstArrayView<ComponentItemInternal*> mv)
  : ComponentItemVectorView(component,mv_indexes,mv){}

  EnvItemVectorView(IMeshComponent* component,ComponentItemVectorView v)
  : ComponentItemVectorView(component,v){}

 private:

  /*!
   * \internal
   * \brief Créé une sous-vue de cette vue.
   * 
   * Cette méthode est interne à Arcane et ne doit pas être utilisée.
   */
  EnvItemVectorView _subView(Integer begin,Integer size);

 public:

  //! Milieu associé
  IMeshEnvironment* environment() const;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arcane::Materials

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  

