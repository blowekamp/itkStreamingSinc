itk_module_test()

#${itk-module} will be the name of this module and will not need to be
#changed when this module is renamed.

set(ITK${itk-module}Tests
  itkStreamingLabelStatisticsImageFilterTest.cxx
  itkStreamingStatisticsImageFilterTest.cxx
  itkStreamingStatisticsImageFilterTest2.cxx
  itkBoundingRegionImageSincTest.cxx
)

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

