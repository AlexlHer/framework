#
# Find the TBB (Intel Thread Building blocks) includes and library
#
# This module defines
# TBB_INCLUDE_DIR, where to find headers,
# TBB_LIBRARIES, the libraries to link against to use TBB.
# TBB_FOUND, If false, do not try to use TBB.

arccon_return_if_package_found(TBB)

find_library(TBB_LIBRARY_DEBUG NAMES tbb_debug)
find_library(TBB_LIBRARY_RELEASE NAMES tbb)
MESSAGE(STATUS "TBB DEBUG ${TBB_LIBRARY_DEBUG}")
MESSAGE(STATUS "TBB RELEASE ${TBB_LIBRARY_RELEASE}")

find_path(TBB_INCLUDE_DIR tbb/tbb_thread.h)

message(STATUS "TBB_INCLUDE_DIR = ${TBB_INCLUDE_DIR}")

if (TBB_LIBRARY_DEBUG)
  set(_TBB_HAS_DEBUG_LIB TRUE)
else()
  set(TBB_LIBRARY_DEBUG ${TBB_LIBRARY_RELEASE})
endif()

set(TBB_FOUND NO)
if (TBB_INCLUDE_DIR AND TBB_LIBRARY_RELEASE AND TBB_LIBRARY_DEBUG)
  set(TBB_FOUND YES)
  if (WIN32)
  else()
    set(TBB_LIBRARIES ${TBB_LIBRARY_RELEASE} )
  endif()
  set(TBB_INCLUDE_DIRS ${TBB_INCLUDE_DIR})
endif()

# Cr�� une interface cible pour r�f�rencer facilement le package dans les d�pendances.
# Utilise TBB_LIBRARY_DEBUG si on compile en mode Debug, TBB_LIBRARY_RELEASE sinon
# En d�bug, il faut aussi d�finir TBB_USE_DEBUG=1 pour que les v�rifications soient
# activ�es.
# TODO: il faudrait pouvoir sp�cifier la version Debug m�me en compilation
# en mode optimis�.
if (TBB_FOUND)
  arccon_register_package_library(TBB TBB)
  if (CMAKE_BUILD_TYPE STREQUAL Debug)
    if (_TBB_HAS_DEBUG_LIB)
      target_compile_definitions(arcconpkg_TBB INTERFACE TBB_USE_DEBUG=1)
    endif()
  endif()
  # Sous Win32, utilise les generator-expression pour sp�cifier le choix de la biblioth�que
  # en fonction de la cible 'Debug' ou 'Release'.
  # Sous Unix, on devrait faire la m�me chose mais cela pose probl�me avec le fichier
  # .pc g�n�r� donc pour l'instant on laisse comme ci dessous.
  if (WIN32)
    target_link_libraries(arcconpkg_TBB INTERFACE "$<$<CONFIG:debug>:${TBB_LIBRARY_DEBUG}>$<$<CONFIG:release>:${TBB_LIBRARY_RELEASE}>")
  else()
    if (CMAKE_BUILD_TYPE STREQUAL Debug)
      target_link_libraries(arcconpkg_TBB INTERFACE ${TBB_LIBRARY_DEBUG})
    else()
      target_link_libraries(arcconpkg_TBB INTERFACE ${TBB_LIBRARY_RELEASE})
    endif()
  endif()
endif (TBB_FOUND)
