itk_module_test()

#${itk-module} will be the name of this module and will not need to be
#changed when this module is renamed.

set(ITK${itk-module}Tests
  itkBoundingRegionImageSincTest.cxx
)

if( ITK_USE_MPI )
  list(APPEND ITK${itk-module}Tests
    itkMPIReadTest.cxx
    itkMPIStreamingImageFilterTest.cxx
    itkMPIStreamingImageFilterTest2.cxx
    )
endif()


CreateTestDriver(${itk-module} "${${itk-module}-Test_LIBRARIES}" "${ITK${itk-module}Tests}")

#PASS_REGULAR_EXPRESSION

itk_add_test(NAME itkBoundingRegionImageSincTest1
  COMMAND ${itk-module}TestDriver --without-threads itkBoundingRegionImageSincTest
  DATA{data/circle.png} 64 )
itk_add_test(NAME itkBoundingRegionImageSincTest2
  COMMAND ${itk-module}TestDriver --with-threads 64 itkBoundingRegionImageSincTest
  DATA{data/circle.png} 1 )
itk_add_test(NAME itkBoundingRegionImageSincTest3
  COMMAND ${itk-module}TestDriver --with-threads 64 itkBoundingRegionImageSincTest
  DATA{data/circle.png} 100 )
set_tests_properties (itkBoundingRegionImageSincTest1
    itkBoundingRegionImageSincTest2
    itkBoundingRegionImageSincTest3
  PROPERTIES
    PASS_REGULAR_EXPRESSION "Region: \\[29, 29\\] \\[87, 87\\]")



#########################################
# MPI

if( ITK_USE_MPI )

add_executable( itkMPITest itkMPITest.cxx )
target_link_libraries( itkMPITest  ${MPI_LIBRARY} ${MPI_EXTRA_LIBRARY}  )

itk_add_test(NAME itkMPITest
  COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS}
    $<TARGET_FILE:itkMPITest> ${MPIEXEC_POSTFLAGS}
   )

itk_add_test(NAME itkMPIStreamingImageFilterTest
  COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS}
    ${MPIEXEC_PREFLAGS} $<TARGET_FILE:${itk-module}TestDriver> ${MPIEXEC_POSTFLAGS}
    itkMPIStreamingImageFilterTest
    DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.mha}
   )

itk_add_test(NAME itkMPIStreamingImageFilterTest2
  COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS}
    ${MPIEXEC_PREFLAGS} $<TARGET_FILE:${itk-module}TestDriver> ${MPIEXEC_POSTFLAGS}
    itkMPIStreamingImageFilterTest2
    DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.mha}
   )


itk_add_test(NAME itkMPIReadTest1
  COMMAND $<TARGET_FILE:${itk-module}TestDriver>
      --compare
        ${ITK_TEST_OUTPUT_DIR}/itkMPIReadTest1.mha
        DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.mha}
    itkMPIReadTest
      DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.mha}
      ${ITK_TEST_OUTPUT_DIR}/itkMPIReadTest1.mha
  )


itk_add_test(NAME itkMPIReadTest2
  COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS}
    ${MPIEXEC_PREFLAGS} $<TARGET_FILE:${itk-module}TestDriver> ${MPIEXEC_POSTFLAGS}
      --compare
        ${ITK_TEST_OUTPUT_DIR}/itkMPIReadTest2.mha
        DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.mha}
    itkMPIReadTest
      DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.mha}
      ${ITK_TEST_OUTPUT_DIR}/itkMPIReadTest2.mha
  )

endif()

