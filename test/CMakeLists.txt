itk_module_test()

#${itk-module} will be the name of this module and will not need to be
#changed when this module is renamed.

set(ITK${itk-module}Tests
  itkStreamingLabelStatisticsImageFilterTest.cxx
)

CreateTestDriver(${itk-module} "${${itk-module}-Test_LIBRARIES}" "${ITK${itk-module}Tests}")

itk_add_test(NAME itkDeleteMeEmptyTest
  COMMAND ${itk-module}TestDriver itkEmptyTest "argument1" "..." )


itk_add_test( NAME itkStreamingLabelStatisticsImageFilterTest1
  COMMAND ${itk-module}TestDriver itkStreamingLabelStatisticsImageFilterTest
  DATA{${ITK_DATA_ROOT}/Input/peppers.png} DATA{${ITK_DATA_ROOT}/Baseline/Algorithms/OtsuMultipleThresholdsImageFilterTest.png} 1)
itk_add_test( NAME itkStreamingLabelStatisticsImageFilterTest2
  COMMAND ${itk-module}TestDriver itkStreamingLabelStatisticsImageFilterTest
  DATA{${ITK_DATA_ROOT}/Input/peppers.png} DATA{${ITK_DATA_ROOT}/Baseline/Algorithms/OtsuMultipleThresholdsImageFilterTest.png} 10)
itk_add_test( NAME itkStreamingLabelStatisticsImageFilterTest3
  COMMAND ${itk-module}TestDriver itkStreamingLabelStatisticsImageFilterTest
 DATA{${ITK_DATA_ROOT}/Input/peppers.png} DATA{${ITK_DATA_ROOT}/Baseline/Algorithms/OtsuMultipleThresholdsImageFilterTest.png} 20)

#PASS_REGULAR_EXPRESSION
