#ifndef __itkMPIStreamingImageFilter_h
#define __itkMPIStreamingImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImageRegionSplitterSlowDimension.h"
#include "itkExtractImageFilter.h"
#include "itkPasteImageFilter.h"
#include <mpi.h>

namespace itk
{
namespace Local
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

  typedef TImageType            ImageType;
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

  void PrintSelf( std::ostream & os, Indent indent ) const;


  virtual void UpdateOutputInformation();

  virtual void GenerateOutputRequestedRegion(DataObject *outputDO);

  virtual void GenerateInputRequestedRegion();

  virtual void GenerateData();

private:
  MPIStreamingImageFilter( const MPIStreamingImageFilter & );  // not implemented
  MPIStreamingImageFilter &operator=( const MPIStreamingImageFilter &);

  int m_MPITAG;
  int m_MPIRank;
  int m_MPISize;
  std::vector< RegionType > m_MPIOutputRegions;
  std::vector< RegionType > m_MPIInputRegions;

  typename SplitterType::Pointer m_RegionSplitter;
};

} // end namespace Local
} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMPIStreamingImageFilter.hxx"
#endif

#endif // __itkMPIStreamingImageFilter_h

