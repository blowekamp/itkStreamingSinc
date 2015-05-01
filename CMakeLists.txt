project(StreamingSinc)
set(StreamingSinc_LIBRARIES StreamingSinc)

if(ITK_USE_MPI)
  set(StreamingSinc_LIBRARIES ${StreamingSinc_LIBRARIES} ${MPI_CXX_LIBRARIES} )
  set(StreamingSinc_SYSTEM_INCLUDE_DIRS ${MPI_CXX_INCLUDE_PATH})
endif()


itk_module_impl()
