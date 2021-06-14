//
// Created by dechaiss on 5/6/20.
//

#ifndef NEO_MESH_H
#define NEO_MESH_H

/*-------------------------
 * sdc - (C) 2020
 * NEtwork Oriented kernel
 * POC Mesh API
 *--------------------------
 */

#include <memory>
#include <string>
#include <vector>

#include "Neo.h"
#include "Utils.h"

namespace Neo {

class MeshBase;
enum class ItemKind;
class Family;
struct FutureItemRange;
class EndOfMeshUpdate;

/*---------------------------------------------------------------------------*/
/*!
 * \brief Asynchronous Mesh API, schedule operations and apply them with
 * \fn applyScheduledOperations
 *
 * When an operation requires an array of external data, since the call to
 * mesh operations is asynchronous the array is copied.
 * To avoid memory copy pass as much as possible your data arrays by rvalue
 * (temporary array or std::move your array);
 *
 */

class Mesh {

public:
  using UidPropertyType   = Neo::PropertyT<Neo::utils::Int64>;
  using CoordPropertyType = Neo::PropertyT<Neo::utils::Real3>;
  using ConnectivityPropertyType = Neo::ArrayProperty<Neo::utils::Int32>;

  struct Connectivity{
    Neo::Family const& source_family;
    Neo::Family const& target_family;
    std::string const& name;
    ConnectivityPropertyType const& connectivity_value;

    Neo::utils::ConstArrayView<Neo::utils::Int32> operator[] (Neo::utils::Int32 item_lid) const noexcept {
      return connectivity_value[item_lid];
    }
  };

  enum class ConnectivityOperation { Add, Modify};

public:
  Mesh(std::string const& mesh_name);
  ~Mesh();

private:
  std::unique_ptr<MeshBase> m_mesh_graph;
  using ConnectivityMapType = std::map<std::string,Connectivity>;
  ConnectivityMapType m_connectivities;

public:
  /*!
   * @brief name of the mesh
   * @return  the name of the mesh
   */
  [[nodiscard]] std::string const& name() const noexcept ;

  /*!
   * @brief mesh dimension
   * @return the dimension of the mesh {1,2,3}
   */
  [[nodiscard]] int dimension() const noexcept {
    return m_mesh_graph->m_dimension;
  }

  /*!
   * @brief mesh node number
   * @return number of nodes in the mesh
   */
  [[nodiscard]] int nbNodes() const noexcept {
    return m_mesh_graph->nbItems(Neo::ItemKind::IK_Node);
  }

  /*!
   * @brief mesh edge number
   * @return number of edges in the mesh
   */
  [[nodiscard]] int nbEdges() const noexcept {
    return m_mesh_graph->nbItems(Neo::ItemKind::IK_Edge);
  }

  /*!
   * @brief mesh face number
   * @return number of faces in the mesh
   */
  [[nodiscard]] int nbFaces() const noexcept {
    return m_mesh_graph->nbItems(Neo::ItemKind::IK_Face);
  }

  /*!
   * @brief mesh cell number
   * @return number of cells in the mesh
   */
  [[nodiscard]] int nbCells() const noexcept {
    return m_mesh_graph->nbItems(Neo::ItemKind::IK_Cell);
  }

  /*!
   * @brief mesh dof number
   * @return number of dofs in the mesh
   */
  [[nodiscard]] int nbDoFs() const noexcept {
    return m_mesh_graph->nbItems(Neo::ItemKind::IK_Dof);
  }

  /*!
   * @brief mesh item with item kind ik number
   * @param ik : kind of the researched item
   * @return number of item with item kind \p ik in the mesh
   */
  [[nodiscard]] int nbItems(Neo::ItemKind ik) const noexcept {
    return m_mesh_graph->nbItems(ik);
  }

  /*!
   * @brief unique id property name for a given family \p family_name
   * @param family_name
   * @return the name of the unique id property for a family with name \p family_name whatever its kind.
   */
  [[nodiscard]] std::string uniqueIdPropertyName(std::string const& family_name) const noexcept ;

  /*!
   * @brief find an existing family given its name \p family_name and kind \p family_kind
   * If the family does not exist
   * @param family_name
   * @param family_kind
   * @return
   */
  Family& findFamily(Neo::ItemKind family_kind, std::string const& family_name) const noexcept(ndebug);

  /*!
   * @brief Add a family of kind \p item_kind and of name \p family_name
   * @param item_kind
   * @param family_name
   * @return
   */
  Neo::Family&  addFamily(Neo::ItemKind item_kind, std::string family_name) noexcept ;

  /*!
   * @brief Schedule items creation for family \p family with unique ids \p uids
   * @param family Family where items are added
   * @param uids Uids of the added items
   * @param future_added_item_range Future ItemRange : after the call to applyScheduledOperations
   * this future range will give access to a range containing new items local ids
   */
  void scheduleAddItems(Neo::Family& family, std::vector<Neo::utils::Int64> uids, Neo::FutureItemRange & future_added_item_range) noexcept ;

  /*!
   * @brief Ask for a fixed-size connectivity add between \p source_family and \p target_family. Source items are scheduled but not created.
   * @param source_family The family of source items.
   * @param source_items Items to be connected. Use of a FutureItemRange means these items come from a the AddItems operation not yet applied.
     * (i.e addItems and addConnectivity are applied with the same call to applyScheduledOperations)
   * @param target_family The family of target items.
   * @param nb_connected_item_per_item Connectivity fix size value.
   * @param connected_item_uids Unique ids of the connected items.
   * @param connectivity_unique_name Connectivity name must be unique
   * @param add_or_modify Indicates whether Connectivity is added or modified (add is default)
   *
   * Connectivity with fix size (nb of connected items per item is constant).
   * Use this method to add connectivity with source items scheduled but not yet created
   * i.e addItems and addConnectivity are applied in the same call to applyScheduledOperations.
   */
  void scheduleAddConnectivity(Neo::Family& source_family, Neo::FutureItemRange& source_items,
                               Neo::Family& target_family, int nb_connected_item_per_item,
                               std::vector<Neo::utils::Int64> connected_item_uids,
                               std::string const& connectivity_unique_name,
                               ConnectivityOperation add_or_modify = ConnectivityOperation::Add);

  /*!
   * @brief Ask for a fixed-size connectivity add between \p source_family and \p target_family. Source items are already created.
   * @param source_family The family of source items.
   * @param source_items Items to be connected. Given via an ItemRange.
     * (i.e addItems and addConnectivity are applied with the same call to applyScheduledOperations)
   * @param target_family The family of target items.
   * @param nb_connected_item_per_item Connectivity fix size value.
   * @param connected_item_uids Unique ids of the connected items.
   * @param connectivity_unique_name Connectivity name must be unique
   * @param add_or_modify Indicates whether Connectivity is added or modified (add is default)
   *
   * Connectivity with fix size (nb of connected items per item is constant).
   * Use this method to add connectivity with source items already created
   * in a previous call to applyScheduledOperations.
   */
  void scheduleAddConnectivity(Neo::Family& source_family, Neo::ItemRange const& source_items,
                               Neo::Family& target_family, int nb_connected_item_per_item,
                               std::vector<Neo::utils::Int64> connected_item_uids,
                               std::string const& connectivity_unique_name,
                               ConnectivityOperation add_or_modify = ConnectivityOperation::Add);

  /*!
   * @brief Ask for a variable size connectivity add between \p source_family and \p target_family. Source items are scheduled but not created.
   * @param source_family The family of source items.
   * @param source_items Items to be connected. Use of a FutureItemRange means these items come from a the AddItems operation not yet applied.
     * (i.e addItems and addConnectivity are applied with the same call to applyScheduledOperations)
   * @param target_family The family of target items.
   * @param nb_connected_item_per_item Number of connected item per items. Array with size equal to source items number.
   * @param connected_item_uids Unique ids of the connected items.
   * @param connectivity_unique_name Connectivity name must be unique
   * @param add_or_modify Indicates whether Connectivity is added or modified (add is default)
   *
   * Connectivity with variable size (nb of connected items per item is variable)
   * Use this method to add connectivity with source items scheduled but not yet created
   * i.e addItems and addConnectivity are applied in the same call to applyScheduledOperations.
   */
  void scheduleAddConnectivity(Neo::Family& source_family, Neo::FutureItemRange& source_items,
                               Neo::Family& target_family, std::vector<int> nb_connected_item_per_item,
                               std::vector<Neo::utils::Int64> connected_item_uids,
                               std::string const& connectivity_unique_name,
                               ConnectivityOperation add_or_modify = ConnectivityOperation::Add);

  /*!
    * @brief Ask for a variable size connectivity add between \p source_family and \p target_family. Source items are already created.
    * @param source_family The family of source items.
    * @param source_items Items to be connected. Use of a FutureItemRange means these items come from a the AddItems operation not yet applied.
      * (i.e addItems and addConnectivity are applied with the same call to applyScheduledOperations)
    * @param target_family The family of target items.
    * @param nb_connected_item_per_item Number of connected item per items. Array with size equal to source items number.
    * @param connected_item_uids Unique ids of the connected items.
    * @param connectivity_unique_name Connectivity name must be unique
    * @param add_or_modify Indicates whether Connectivity is added or modified (add is default)
    *
    * Connectivity with variable size (nb of connected items per item is variable)
    * Use this method to add connectivity with source items already created
    * in a previous call to applyScheduledOperations.
    */
  void scheduleAddConnectivity(Neo::Family& source_family, Neo::ItemRange const& source_items,
                               Neo::Family& target_family, std::vector<int> nb_connected_item_per_item,
                               std::vector<Neo::utils::Int64> connected_item_uids,
                               std::string const& connectivity_unique_name,
                               ConnectivityOperation add_or_modify = ConnectivityOperation::Add);

  /*!
   * @brief Schedule an set item coordinates. Will be applied when applyScheduledOperations will be called
   * @param item_family Family of the items whose coords will be modified
   * @param future_added_item_range Set of the items whose coords will be modified. These items are not created yet.
   * They will be created in applyScheduledOperations before the call to this coords set.
   * @param item_coords Value of the items coordinates
   */
  void scheduleSetItemCoords(Neo::Family& item_family, Neo::FutureItemRange& future_added_item_range,std::vector<Neo::utils::Real3> item_coords) noexcept ;

  /*!
   * @brief Get item connectivity between \p source_family and \p target_family with name \p name
   * @param source_family Family of the source items
   * @param target_family Family of the target items
   * @param connectivity_name Name of the connectivity
   * @return Connectivity, a connectivity wrapper object
   * @throw a std::invalid_argument if the connectivity is not found
   */
  Connectivity const getConnectivity(Neo::Family const& source_family,Neo::Family const& target_family,std::string const& connectivity_name);

  /*!
   * @brief Apply all scheduled operations (addItems, addConnectivities, setItemCoords)
   * @return An object allowing to get the new items ItemRange from the FutureItemRange
   */
  Neo::EndOfMeshUpdate applyScheduledOperations() noexcept ;

  /*!
   * Use this method to change coordinates of existing items
   * @param family ItemFamily of item to change coordinates
   * @return Coordinates property. Usage : Real3 coord = coord_prop[item_lid];
   */
  [[nodiscard]] CoordPropertyType& getItemCoordProperty(Neo::Family & family);
  [[nodiscard]] CoordPropertyType const& getItemCoordProperty(Neo::Family const& family) const;

  /*!
   * Get unique id property of a family
   * @param item_family ItemFamily of concerned items
   * @return UniqueId property. Udage Neo::utils::Int64 uid = uid_prop[item_lid]
   *
   * A more direct usage is to call directy \fn uniqueIds(item_family,item_lids)
   * to get uids of given lids
   */
  [[nodiscard]] UidPropertyType const & getItemUidsProperty(const Family &item_family) const noexcept;

  /*!
   * @brief Get items of kind \p item_kind connected to family \p source_family
   * @param source_family Family of source items
   * @param item_kind Kind of connected items
   * @return A vector of all Connectivities connecting \p source_family to a target family with kind \p item_kind
   */
  std::vector<Connectivity> items(Neo::Family const& source_family, Neo::ItemKind item_kind) const noexcept;

  std::vector<Connectivity> edges(Neo::Family const& source_family) const noexcept;
  std::vector<Connectivity> nodes(Neo::Family const& source_family) const noexcept;
  std::vector<Connectivity> faces(Neo::Family const& source_family) const noexcept;
  std::vector<Connectivity> cells(Neo::Family const& source_family) const noexcept;

  std::vector<Connectivity> dofs(Neo::Family const& source_family) const noexcept;

  /*!
   * Get unique ids from \p item_lids (local ids) in \p item_family
   * @param item_family Family of given items
   * @param item_lids Given item local ids
   * @return Given item unique ids
   */
  std::vector<Neo::utils::Int64> uniqueIds(Family const &item_family,const std::vector<Neo::utils::Int32> &item_lids) const noexcept ;
  /*!
   * Get local ids from \p item_uids (unique ids) in \p item_family
   * @param item_family Family of given items
   * @param item_uids Given item unique ids
   * @return Given item local ids
   */
  std::vector<Neo::utils::Int32> localIds(Family const &item_family,const std::vector<Neo::utils::Int64> &item_uids) const noexcept ;

private:

  [[nodiscard]] std::string _itemCoordPropertyName(Family const& item_family) const {return item_family.name()+"_item_coordinates";}
};

} // end namespace Neo

#endif // NEO_MESH_H
