/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <iostream>

#include "itkImage.h"
#include "itkStreamingStatisticsImageFilter.h"
#include "itkImageFileReader.h"

#include "itkFilterWatcher.h"

#include "vnl/vnl_math.h"

int itkStreamingStatisticsImageFilterTest2(int argc, char* argv [] )
{

  if( argc < 3 )
  {
    std::cerr << "Missing Arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImage numberOfStreamDivisions" << std::endl;
    return EXIT_FAILURE;
  }

  unsigned int numberOfStreamDivisions = std::max( atoi( argv[2] ), 1 );

  typedef itk::Image<float,2> ImageType;

  typedef itk::ImageFileReader< ImageType >    ReaderType;

  ReaderType::Pointer reader1 = ReaderType::New();

  reader1->SetFileName( argv[1] );

  typedef itk::StreamingStatisticsImageFilter< ImageType > FilterType;

  FilterType::Pointer filter = FilterType::New();
  filter->DebugOn();

  FilterWatcher filterWatch( filter );

  filter->SetInput (      reader1->GetOutput() );
  filter->SetNumberOfStreamDivisions( numberOfStreamDivisions );
  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception caught ! " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  typedef FilterType::PixelType PixelType;
  typedef FilterType::RealType  RealType;

  const PixelType min      =  filter->GetMinimum( );
  const PixelType max      =  filter->GetMaximum();
  const RealType mean     =  filter->GetMean( );
  const RealType sigma    =  filter->GetSigma();
  const RealType variance =  filter->GetVariance();
  const RealType sum      =  filter->GetSum();

  std::cout << "Result Minimum   = " << min      << std::endl;
  std::cout << "Result Maximum   = " << max      << std::endl;
  std::cout << "Result Mean      = " << mean     << std::endl;
  std::cout << "Result Sigma     = " << sigma    << std::endl;
  std::cout << "Result Variance  = " << variance << std::endl;
  std::cout << "Result Sum       = " << sum      << std::endl;

  return EXIT_SUCCESS;
}
