﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2023 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* VtkHdfPostProcessor.cc                                      (C) 2000-2023 */
/*                                                                           */
/* Pos-traitement au format VTK HDF.                                         */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/utils/Collection.h"
#include "arcane/utils/Enumerator.h"
#include "arcane/utils/Iostream.h"
#include "arcane/utils/ScopedPtr.h"
#include "arcane/utils/StringBuilder.h"
#include "arcane/utils/CheckedConvert.h"
#include "arcane/utils/JSONWriter.h"
#include "arcane/utils/IOException.h"

#include "arcane/core/PostProcessorWriterBase.h"
#include "arcane/core/Directory.h"
#include "arcane/core/FactoryService.h"
#include "arcane/core/IDataWriter.h"
#include "arcane/core/IData.h"
#include "arcane/core/IItemFamily.h"
#include "arcane/core/VariableCollection.h"
#include "arcane/core/IParallelMng.h"
#include "arcane/core/IMesh.h"

#include "arcane/std/Hdf5Utils.h"
#include "arcane/std/VtkHdfPostProcessor_axl.h"
#include "arcane/std/internal/VtkCellTypes.h"

// Ce format est décrit sur la page web suivante:
//
// https://kitware.github.io/vtk-examples/site/VTKFileFormats/#hdf-file-formats

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

// TODO: Supporter le parallélisme
// TODO: Regarder la sauvegarde des uniqueId() (via vtkOriginalCellIds)
// TODO: Regarder comment éviter de sauver le maillage à chaque itération s'il
//       ne change pas. Avec la notion de lien de HDF5, il doit être possible
//       de mettre cette information ailleurs et de ne sauver le maillage
//       que s'il évolue.
// TODO: Regarder la compression

/*
  NOTE sur l'implémentation parallèle

  L'implémentation actuelle est très basique.
  Seul le rang maitre (le rang 0 en général) effectue les sorties. Pour
  chaque dataset, ce rang fait un gather pour récupérer les informations. Cela
  suppose donc que tout le monde a les mêmes variables et dans le même ordre
  (normalement c'est toujours le cas car c'est trié par la VariableMng).

  Cette implémentation fonctionne donc quel que soit le mode d'échange de
  message utilisé (full MPI, mémoire partagé ou hybride).

  TODO: Découpler l'écriture de la gestion des variables pour pouvoir utiliser
  les opérations collectives de HDF5 (si compilé avec MPI). Dans ce cas il
  faut quand même géré manuellement le mode échange de message en mémoire
  partagée ou hybride.

  TODO: Ajouter informations sur les mailles fantômes
*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane
{
using namespace Hdf5Utils;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class VtkHdfDataWriter
: public TraceAccessor
, public IDataWriter
{
 public:

  VtkHdfDataWriter(IMesh* mesh, ItemGroupCollection groups);

 public:

  void beginWrite(const VariableCollection& vars) override;
  void endWrite() override;
  void setMetaData(const String& meta_data) override;
  void write(IVariable* var, IData* data) override;

 public:

  void setTimes(RealConstArrayView times) { m_times = times; }
  void setDirectoryName(const String& dir_name) { m_directory_name = dir_name; }

 private:

  IMesh* m_mesh;

  //! Liste des groupes à sauver
  ItemGroupCollection m_groups;

  //! Liste des temps
  UniqueArray<Real> m_times;

  //! Nom du fichier HDF courant
  String m_full_filename;

  //! Répertoire de sortie.
  String m_directory_name;

  //! Identifiant HDF du fichier
  HFile m_file_id;

  HGroup m_cell_data_group;
  HGroup m_node_data_group;
  bool m_is_parallel = false;
  bool m_is_master_io = false;

 private:

  void _addInt64ArrayAttribute(Hid& hid, const char* name, Span<const Int64> values);
  void _addStringAttribute(Hid& hid, const char* name, const String& value);

  template <typename DataType> void
  _writeDataSet1D(HGroup& group, const String& name, Span<const DataType> values);
  template <typename DataType> void
  _writeDataSet1DCollective(HGroup& group, const String& name, Span<const DataType> values);
  template <typename DataType> void
  _writeDataSet2D(HGroup& group, const String& name, Span2<const DataType> values);
  template <typename DataType> void
  _writeDataSet2DCollective(HGroup& group, const String& name, Span2<const DataType> values);
  template <typename DataType> void
  _writeBasicTypeDataset(HGroup& group, IVariable* var, IData* data);
  void _writeReal3Dataset(HGroup& group, IVariable* var, IData* data);
  void _writeReal2Dataset(HGroup& group, IVariable* var, IData* data);

  String _getFileNameForTimeIndex(Int32 index)
  {
    StringBuilder sb(m_mesh->name());
    if (index >= 0) {
      sb += "_";
      sb += index;
    }
    sb += ".hdf";
    return sb.toString();
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

VtkHdfDataWriter::
VtkHdfDataWriter(IMesh* mesh, ItemGroupCollection groups)
: TraceAccessor(mesh->traceMng())
, m_mesh(mesh)
, m_groups(groups)
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void VtkHdfDataWriter::
beginWrite(const VariableCollection& vars)
{
  ARCANE_UNUSED(vars);

  IParallelMng* pm = m_mesh->parallelMng();
  const Int32 nb_rank = pm->commSize();
  m_is_parallel = nb_rank > 1;
  m_is_master_io = pm->isMasterIO();

  Int32 time_index = m_times.size();
  if (time_index < 2)
    pwarning() << "L'implémentation au format 'VtkHdf' est expérimentale";

  String filename = _getFileNameForTimeIndex(time_index);

  Directory dir(m_directory_name);

  m_full_filename = dir.file(filename);
  info(4) << "VtkHdfDataWriter::beginWrite() file=" << m_full_filename;

  HInit();

  HGroup top_group;

  if (m_is_master_io) {
    if (time_index <= 1)
      dir.createDirectory();
    m_file_id.openTruncate(m_full_filename);
    top_group.create(m_file_id, "VTKHDF");

    m_cell_data_group.create(top_group, "CellData");
    m_node_data_group.create(top_group, "PointData");

    std::array<Int64, 2> version = { 1, 0 };
    _addInt64ArrayAttribute(top_group, "Version", version);

    _addStringAttribute(top_group, "Type", "UnstructuredGrid");
  }

  CellGroup all_cells = m_mesh->allCells();
  NodeGroup all_nodes = m_mesh->allNodes();

  const Int32 nb_cell = all_cells.size();
  const Int32 nb_node = all_nodes.size();

  // TODO: dimensionner directement à la bonne taille
  // Pour les connectivités, la taille du tableau est égal
  // au nombre de mailes plus 1.
  UniqueArray<Int64> cells_connectivity;
  UniqueArray<Int64> cells_offset;
  UniqueArray<unsigned char> cells_type;
  UniqueArray<unsigned char> cells_ghost_type;
  cells_offset.add(0);
  ENUMERATE_CELL (icell, all_cells) {
    Cell cell = *icell;
    Byte ghost_type = 0;
    bool is_ghost = !cell.isOwn();
    if (is_ghost)
      ghost_type = VtkUtils::CellGhostTypes::DUPLICATECELL;
    cells_ghost_type.add(ghost_type);
    unsigned char vtk_type = VtkUtils::arcaneToVtkCellType(cell.type());
    cells_type.add(vtk_type);
    for (NodeLocalId node : cell.nodeIds())
      cells_connectivity.add(node);
    cells_offset.add(cells_connectivity.size());
  }

  _writeDataSet1DCollective<Int64>(top_group, "Offsets", cells_offset);
  _writeDataSet1DCollective<Int64>(top_group, "Connectivity", cells_connectivity);
  _writeDataSet1DCollective<unsigned char>(top_group, "Types", cells_type);

  UniqueArray<Int64> nb_cell_by_ranks(1);
  nb_cell_by_ranks[0] = nb_cell;
  _writeDataSet1DCollective<Int64>(top_group, "NumberOfCells", nb_cell_by_ranks);

  UniqueArray<Int64> nb_node_by_ranks(1);
  nb_node_by_ranks[0] = nb_node;
  _writeDataSet1DCollective<Int64>(top_group, "NumberOfPoints", nb_node_by_ranks);

  UniqueArray<Int64> number_of_connectivity_ids(1);
  number_of_connectivity_ids[0] = cells_connectivity.size();
  _writeDataSet1DCollective<Int64>(top_group, "NumberOfConnectivityIds", number_of_connectivity_ids);

  VariableNodeReal3& nodes_coordinates(m_mesh->nodesCoordinates());
  UniqueArray2<Real> points;
  points.resize(nb_node, 3);
  ENUMERATE_NODE (inode, all_nodes) {
    Int32 index = inode.index();
    Real3 pos = nodes_coordinates[inode];
    points[index][0] = pos.x;
    points[index][1] = pos.y;
    points[index][2] = pos.z;
  }
  _writeDataSet2DCollective<Real>(top_group, "Points", points);

  _writeDataSet1DCollective<unsigned char>(m_cell_data_group, "vtkGhostType", cells_ghost_type);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace
{
  template <typename DataType> class HDFTraits;

  template <> class HDFTraits<Int64>
  {
   public:

    static hid_t hdfType() { return H5T_NATIVE_INT64; }
  };

  template <> class HDFTraits<Int32>
  {
   public:

    static hid_t hdfType() { return H5T_NATIVE_INT32; }
  };

  template <> class HDFTraits<double>
  {
   public:

    static hid_t hdfType() { return H5T_NATIVE_DOUBLE; }
  };

  template <> class HDFTraits<unsigned char>
  {
   public:

    static hid_t hdfType() { return H5T_NATIVE_UINT8; }
  };

} // namespace

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template <typename DataType> void VtkHdfDataWriter::
_writeDataSet1D(HGroup& group, const String& name, Span<const DataType> values)
{
  hsize_t dims[1];
  dims[0] = values.size();
  HSpace hspace;
  hspace.createSimple(1, dims);
  HDataset dataset;
  const hid_t hdf_type = HDFTraits<DataType>::hdfType();
  dataset.create(group, name.localstr(), hdf_type, hspace, H5P_DEFAULT);
  dataset.write(hdf_type, values.data());
  if (dataset.isBad())
    ARCANE_THROW(IOException, "Can not write dataset '{0}'", name);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template <typename DataType> void VtkHdfDataWriter::
_writeDataSet1DCollective(HGroup& group, const String& name, Span<const DataType> values)
{
  if (!m_is_parallel) {
    _writeDataSet1D(group, name, values);
    return;
  }

  UniqueArray<DataType> all_values;
  IParallelMng* pm = m_mesh->parallelMng();
  pm->gatherVariable(values.smallView(), all_values, pm->masterIORank());
  if (m_is_master_io)
    _writeDataSet1D<DataType>(group, name, all_values);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template <typename DataType> void VtkHdfDataWriter::
_writeDataSet2D(HGroup& group, const String& name, Span2<const DataType> values)
{
  hsize_t dims[2];
  dims[0] = values.dim1Size();
  dims[1] = values.dim2Size();
  HSpace hspace;
  hspace.createSimple(2, dims);
  HDataset dataset;
  const hid_t hdf_type = HDFTraits<DataType>::hdfType();
  dataset.create(group, name.localstr(), hdf_type, hspace, H5P_DEFAULT);
  dataset.write(hdf_type, values.data());
  if (dataset.isBad())
    ARCANE_THROW(IOException, "Can not write dataset '{0}'", name);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template <typename DataType> void VtkHdfDataWriter::
_writeDataSet2DCollective(HGroup& group, const String& name, Span2<const DataType> values)
{
  Int64 dim2_size = values.dim2Size();

  if (!m_is_parallel) {
    _writeDataSet2D(group, name, values);
    return;
  }

  UniqueArray<DataType> all_values;
  IParallelMng* pm = m_mesh->parallelMng();
  Span<const DataType> values_1d(values.data(), values.totalNbElement());
  pm->gatherVariable(values_1d.smallView(), all_values, pm->masterIORank());
  if (m_is_master_io) {
    Int64 dim1_size = all_values.size();
    if (dim2_size != 0)
      dim1_size = dim1_size / dim2_size;
    Span2<const DataType> span2(all_values.data(), dim1_size, dim2_size);
    _writeDataSet2D<DataType>(group, name, span2);
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void VtkHdfDataWriter::
_addInt64ArrayAttribute(Hid& hid, const char* name, Span<const Int64> values)
{
  hsize_t len = values.size();
  hid_t aid = H5Screate_simple(1, &len, 0);
  hid_t attr = H5Acreate2(hid.id(), name, H5T_NATIVE_INT64, aid, H5P_DEFAULT, H5P_DEFAULT);
  if (attr < 0)
    ARCANE_FATAL("Can not create attribute '{0}'", name);
  int ret = H5Awrite(attr, H5T_NATIVE_INT64, values.data());
  if (ret < 0)
    ARCANE_FATAL("Can not write attribute '{0}'", name);
  H5Aclose(attr);
  H5Sclose(aid);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void VtkHdfDataWriter::
_addStringAttribute(Hid& hid, const char* name, const String& value)
{
  hid_t aid = H5Screate(H5S_SCALAR);
  hid_t attr_type = H5Tcopy(H5T_C_S1);
  H5Tset_size(attr_type, value.length());
  hid_t attr = H5Acreate2(hid.id(), name, attr_type, aid, H5P_DEFAULT, H5P_DEFAULT);
  if (attr < 0)
    ARCANE_FATAL("Can not create attribute {0}", name);
  int ret = H5Awrite(attr, attr_type, value.localstr());
  ret = H5Tclose(attr_type);
  if (ret < 0)
    ARCANE_FATAL("Can not write attribute '{0}'", name);
  H5Aclose(attr);
  H5Sclose(aid);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void VtkHdfDataWriter::
endWrite()
{
  m_file_id.close();

  // Ecrit le fichier contenant les temps (à partir de la version 5.5 de paraview)
  // https://www.paraview.org/Wiki/ParaView_Release_Notes#JSON_based_new_meta_file_format_for_series_added
  //
  // Exemple:
  // {
  //   "file-series-version" : "1.0",
  //   "files" : [
  //     { "name" : "foo1.vtk", "time" : 0 },
  //     { "name" : "foo2.vtk", "time" : 5.5 },
  //     { "name" : "foo3.vtk", "time" : 11.2 }
  //   ]
  // }

  if (!m_is_master_io)
    return;

  JSONWriter writer(JSONWriter::FormatFlags::None);
  {
    JSONWriter::Object o(writer);
    writer.write("file-series-version", "1.0");
    writer.writeKey("files");
    writer.beginArray();
    {
      Integer file_index = 1;
      for (Real v : m_times) {
        JSONWriter::Object o(writer);
        String filename = _getFileNameForTimeIndex(file_index);
        writer.write("name", filename);
        writer.write("time", v);
        ++file_index;
      }
      writer.endArray();
    }
  }
  Directory dir(m_directory_name);
  String fname = dir.file(_getFileNameForTimeIndex(-1) + ".series");
  std::ofstream ofile(fname.localstr());
  StringView buf = writer.getBuffer();
  ofile.write(reinterpret_cast<const char*>(buf.bytes().data()), buf.length());
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void VtkHdfDataWriter::
setMetaData(const String& meta_data)
{
  ARCANE_UNUSED(meta_data);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void VtkHdfDataWriter::
write(IVariable* var, IData* data)
{
  info(4) << "Write VtkHdf var=" << var->name();

  eItemKind item_kind = var->itemKind();

  if (var->dimension() != 1)
    ARCANE_FATAL("Only export of scalar item variable is implemented (name={0})", var->name());

  HGroup* group = nullptr;
  switch (item_kind) {
  case IK_Cell:
    group = &m_cell_data_group;
    break;
  case IK_Node:
    group = &m_node_data_group;
    break;
  default:
    ARCANE_FATAL("Only export of 'Cell' or 'Node' variable is implemented (name={0})", var->name());
  }
  ARCANE_CHECK_POINTER(group);

  eDataType data_type = var->dataType();
  switch (data_type) {
  case DT_Real:
    _writeBasicTypeDataset<Real>(*group, var, data);
    break;
  case DT_Int64:
    _writeBasicTypeDataset<Real>(*group, var, data);
    break;
  case DT_Int32:
    _writeBasicTypeDataset<Real>(*group, var, data);
    break;
  case DT_Real3:
    _writeReal3Dataset(*group, var, data);
    break;
  case DT_Real2:
    _writeReal2Dataset(*group, var, data);
    break;
  default:
    warning() << String::format("Export for datatype '{0}' is not supported (var_name={1})", data_type, var->name());
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template <typename DataType> void VtkHdfDataWriter::
_writeBasicTypeDataset(HGroup& group, IVariable* var, IData* data)
{
  auto* true_data = dynamic_cast<IArrayDataT<DataType>*>(data);
  ARCANE_CHECK_POINTER(true_data);
  _writeDataSet1DCollective(group, var->name(), Span<const DataType>(true_data->view()));
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void VtkHdfDataWriter::
_writeReal3Dataset(HGroup& group, IVariable* var, IData* data)
{
  auto* true_data = dynamic_cast<IArrayDataT<Real3>*>(data);
  ARCANE_CHECK_POINTER(true_data);
  SmallSpan<const Real3> values(true_data->view());
  Int32 nb_value = values.size();
  // TODO: optimiser cela sans passer par un tableau temporaire
  UniqueArray2<Real> scalar_values;
  scalar_values.resize(nb_value, 3);
  for (Int32 i = 0; i < nb_value; ++i) {
    Real3 v = values[i];
    scalar_values[i][0] = v.x;
    scalar_values[i][1] = v.y;
    scalar_values[i][2] = v.z;
  }
  _writeDataSet2DCollective<Real>(group, var->name(), scalar_values);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void VtkHdfDataWriter::
_writeReal2Dataset(HGroup& group, IVariable* var, IData* data)
{
  // Converti en un tableau de 3 composantes dont la dernière vaudra 0.
  auto* true_data = dynamic_cast<IArrayDataT<Real2>*>(data);
  ARCANE_CHECK_POINTER(true_data);
  SmallSpan<const Real2> values(true_data->view());
  Int32 nb_value = values.size();
  UniqueArray2<Real> scalar_values;
  scalar_values.resize(nb_value, 3);
  for (Int32 i = 0; i < nb_value; ++i) {
    Real2 v = values[i];
    scalar_values[i][0] = v.x;
    scalar_values[i][1] = v.y;
    scalar_values[i][2] = 0.0;
  }
  _writeDataSet2DCollective<Real>(group, var->name(), scalar_values);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Post-traitement au format Ensight Hdf.
 */
class VtkHdfPostProcessor
: public ArcaneVtkHdfPostProcessorObject
{
 public:

  explicit VtkHdfPostProcessor(const ServiceBuildInfo& sbi)
  : ArcaneVtkHdfPostProcessorObject(sbi)
  {
  }

  IDataWriter* dataWriter() override { return m_writer.get(); }
  void notifyBeginWrite() override
  {
    auto w = std::make_unique<VtkHdfDataWriter>(mesh(), groups());
    w->setTimes(times());
    Directory dir(baseDirectoryName());
    w->setDirectoryName(dir.file("vtkhdf"));
    m_writer = std::move(w);
  }
  void notifyEndWrite() override
  {
    m_writer = nullptr;
  }
  void close() override {}

 private:

  std::unique_ptr<IDataWriter> m_writer;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SUB_DOMAIN_FACTORY(VtkHdfPostProcessor,
                                   IPostProcessorWriter,
                                   VtkHdfPostProcessor);

ARCANE_REGISTER_SERVICE_VTKHDFPOSTPROCESSOR(VtkHdfPostProcessor,
                                            VtkHdfPostProcessor);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // namespace Arcane

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
