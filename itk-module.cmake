# the top-level README is used for describing this module, just
# re-used it for documentation here
get_filename_component( MY_CURENT_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
file( READ "${MY_CURENT_DIR}/README.rst" DOCUMENTATION )

# ITK version 4.5 changed it from EXCLUDE_FROM_ALL to EXCLUDE_FROM_DEFAULT
set( _EXCLUDE "EXCLUDE_FROM_ALL" )
if (NOT "${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}.${ITK_VERSION_MINOR_PATCH}" VERSION_LESS "4.5")
  set( _EXCLUDE "EXCLUDE_FROM_DEFAULT" )
endif()


# itk_module() defines the module dependencies in StreamingSinc
# StreamingSinc depends on ITKCommon
# The testing module in ITKStreamingSinc depends on ITKTestKernel
# and ITKMetaIO(besides ITKStreamingSinc and ITKCore)

# define the dependencies of the include module and the tests
itk_module(StreamingSinc
  DEPENDS
    ITKCommon
    ITKImageFilterBase
    ITKStatistics
    ITKImageStatistics
    ITKImageGrid
  TEST_DEPENDS
    ITKTestKernel
    ITKStatistics
    ITKSmoothing
  DESCRIPTION
    "${DOCUMENTATION}"
  EXCLUDE_FROM_DEFAULT
)
