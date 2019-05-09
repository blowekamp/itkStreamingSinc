cmake_minimum_required(VERSION 3.10.2)
project(StreamingSinc)

set(StreamingSinc_LIBRARIES StreamingSinc)

if(ITK_USE_MPI)
  set(StreamingSinc_LIBRARIES ${StreamingSinc_LIBRARIES} ${MPI_CXX_LIBRARIES} )
  set(StreamingSinc_SYSTEM_INCLUDE_DIRS ${MPI_CXX_INCLUDE_PATH})
endif()

if(NOT ITK_SOURCE_DIR)
  find_package(ITK REQUIRED)
  list(APPEND CMAKE_MODULE_PATH ${ITK_CMAKE_DIR})
  include(ITKModuleExternal)
else()
  itk_module_impl()
endif()

