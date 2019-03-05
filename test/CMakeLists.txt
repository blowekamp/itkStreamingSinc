itk_module_test()

#${itk-module} will be the name of this module and will not need to be
#changed when this module is renamed.

set(ITK${itk-module}Tests
  itkStreamingLabelStatisticsImageFilterTest.cxx
  itkStreamingStatisticsImageFilterTest.cxx
  itkStreamingStatisticsImageFilterTest2.cxx
  itkStreamingImageToHistogramFilterTest.cxx
  itkStreamingImageToHistogramFilterTest2.cxx
  itkStreamingImageToHistogramFilterTest3.cxx
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


itk_add_test( NAME itkStreamingLabelStatisticsImageFilterTest1
  COMMAND ${itk-module}TestDriver itkStreamingLabelStatisticsImageFilterTest
  DATA{${ITK_DATA_ROOT}/Input/peppers.png} DATA{${ITK_DATA_ROOT}/Baseline/Algorithms/OtsuMultipleThresholdsImageFilterTest.png} 1)
itk_add_test( NAME itkStreamingLabelStatisticsImageFilterTest2
  COMMAND ${itk-module}TestDriver itkStreamingLabelStatisticsImageFilterTest
  DATA{${ITK_DATA_ROOT}/Input/peppers.png} DATA{${ITK_DATA_ROOT}/Baseline/Algorithms/OtsuMultipleThresholdsImageFilterTest.png} 10)
itk_add_test( NAME itkStreamingLabelStatisticsImageFilterTest3
  COMMAND ${itk-module}TestDriver itkStreamingLabelStatisticsImageFilterTest
 DATA{${ITK_DATA_ROOT}/Input/peppers.png} DATA{${ITK_DATA_ROOT}/Baseline/Algorithms/OtsuMultipleThresholdsImageFilterTest.png} 20)

itk_add_test(NAME itkStreamingStatisticsImageFilterTest
      COMMAND ITKImageStatisticsTestDriver itkStatisticsImageFilterTest)
itk_add_test( NAME itkStreamingStatisticsImageFilterTest2_1
  COMMAND ${itk-module}TestDriver itkStreamingStatisticsImageFilterTest2
  DATA{${ITK_DATA_ROOT}/Input/peppers.png}  1)
itk_add_test( NAME itkStreamingStatisticsImageFilterTest2_2
  COMMAND ${itk-module}TestDriver itkStreamingStatisticsImageFilterTest2
  DATA{${ITK_DATA_ROOT}/Input/peppers.png} 10)
itk_add_test( NAME itkStreamingStatisticsImageFilterTest2_3
  COMMAND ${itk-module}TestDriver itkStreamingStatisticsImageFilterTest2
 DATA{${ITK_DATA_ROOT}/Input/peppers.png} 20)

itk_add_test(NAME itkStreamingImageToHistogramFilterTest
      COMMAND ITKImageStatisticsTestDriver itkStreamingImageToHistogramFilterTest)
itk_add_test(NAME itkStreamingImageToHistogramFilterTest2
      COMMAND ITKImageStatisticsTestDriver itkStreamingImageToHistogramFilterTest2
              DATA{${ITK_DATA_ROOT}/Input/VisibleWomanEyeSlice.png} ${ITK_TEST_OUTPUT_DIR}/itkStreamingImageToHistogramFilterTest2.txt)
itk_add_test(NAME itkStreamingImageToHistogramFilterTest3
      COMMAND ITKImageStatisticsTestDriver itkStreamingImageToHistogramFilterTest3
              DATA{${ITK_DATA_ROOT}/Input/cthead1.png} ${ITK_TEST_OUTPUT_DIR}/itkStreamingImageToHistogramFilterTest3.txt)

#PASS_REGULAR_EXPRESSION

itk_add_test(NAME itkBoundingRegionImageSincTest1
  COMMAND ${itk-module}TestDriver --without-threads itkBoundingRegionImageSincTest
  DATA{${ITK_DATA_ROOT}/Input/circle.png} 64 )
itk_add_test(NAME itkBoundingRegionImageSincTest2
  COMMAND ${itk-module}TestDriver --with-threads 64 itkBoundingRegionImageSincTest
  DATA{${ITK_DATA_ROOT}/Input/circle.png} 1 )
itk_add_test(NAME itkBoundingRegionImageSincTest3
  COMMAND ${itk-module}TestDriver --with-threads 64 itkBoundingRegionImageSincTest
  DATA{${ITK_DATA_ROOT}/Input/circle.png} 100 )
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

