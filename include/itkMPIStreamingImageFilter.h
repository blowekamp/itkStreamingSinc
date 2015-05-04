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
#ifndef itkMPIStreamingImageFilter_h
#define itkMPIStreamingImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImageRegionSplitterSlowDimension.h"
#include "itkExtractImageFilter.h"
#include "itkPasteImageFilter.h"
#include <mpi.h>

namespace itk
{

template < class TImageType >
class MPIStreamingImageFilter
  : public ImageToImageFilter< TImageType, TImageType >
{
public:
  /** Standard class typedefs. */
  typedef MPIStreamingImageFilter                         Self;
  typedef ImageToImageFilter< TImageType, TImageType >    Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;

  typedef TImageType                     ImageType;
  typedef typename ImageType::RegionType RegionType;

   /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MPIStreamingImageFilter, ImageToImageFilter);

  /** A region splitting object base */
  typedef ImageRegionSplitterBase SplitterType;

  /** Set the helper class for dividing the input into chunks. */
  itkSetObjectMacro(RegionSplitter, SplitterType);
  itkGetObjectMacro(RegionSplitter, SplitterType);

protected:
  MPIStreamingImageFilter();
  ~MPIStreamingImageFilter();

  void PrintSelf( std::ostream & os, Indent indent ) const ITK_OVERRIDE;

  virtual void UpdateOutputInformation() ITK_OVERRIDE;

  virtual void GenerateOutputRequestedRegion(DataObject *outputDO) ITK_OVERRIDE;

  virtual void GenerateInputRequestedRegion() ITK_OVERRIDE;

  virtual void GenerateData() ITK_OVERRIDE;

  static MPI_Datatype GetMPIDataTypeForPixel()
    {


#define DefineMPITypeHelper( TYPE, MPI_VALUE ) \
      if ( IsSame<TYPE, typename TImageType::PixelType>::Value )  { return MPI_VALUE; }

      DefineMPITypeHelper( float, MPI_FLOAT );
      DefineMPITypeHelper( double, MPI_DOUBLE );

      DefineMPITypeHelper( char, MPI_CHAR );
      DefineMPITypeHelper( unsigned char, MPI_UNSIGNED_CHAR );
      DefineMPITypeHelper( short, MPI_SHORT );
      DefineMPITypeHelper( unsigned short, MPI_UNSIGNED_SHORT );
      DefineMPITypeHelper( int, MPI_INT );
      DefineMPITypeHelper( unsigned int, MPI_UNSIGNED );
      DefineMPITypeHelper( long, MPI_LONG );
      DefineMPITypeHelper( unsigned long, MPI_UNSIGNED_LONG );
      DefineMPITypeHelper( long long, MPI_LONG_LONG_INT );
      DefineMPITypeHelper( unsigned long long, MPI_UNSIGNED_LONG_LONG );

#undef DefineMPITypeHelper
      itkGenericExceptionMacro("Unsupported PixelType");
    }

private:
  MPIStreamingImageFilter( const MPIStreamingImageFilter & );  // not implemented
  MPIStreamingImageFilter &operator=( const MPIStreamingImageFilter &);

  int m_MPITAG;
  int m_MPIRank;
  int m_MPISize;

  const MPI_Datatype m_MPIDataType;

  std::vector< RegionType > m_MPIOutputRegions;
  std::vector< RegionType > m_MPIInputRegions;

  typename SplitterType::Pointer m_RegionSplitter;
};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMPIStreamingImageFilter.hxx"
#endif

#endif // itkMPIStreamingImageFilter_h
