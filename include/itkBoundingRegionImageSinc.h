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
#ifndef itkBoundingRegionImageSinc_h
#define itkBoundingRegionImageSinc_h

#include "itkImageSink.h"
#include "itkImageScanlineIterator.h"
#include "itkSimpleDataObjectDecorator.h"
#include <numeric>
#include <mutex>

namespace itk
{

/** \class BoundingRegionImageSinc
 *
 * \ingroup StreamingSinc
 **/
template< class TInputImage >
class BoundingRegionImageSinc
  : public ImageSink<TInputImage>
{
public:
  /** Standard class typedefs. */
  typedef BoundingRegionImageSinc     Self;
  typedef ImageSink< TInputImage >    Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(BoundingRegionImageSinc, ImageSink);

  /** Image type information. */
  typedef typename Superclass::InputImageType       InputImageType;
  typedef typename Superclass::InputImageRegionType RegionType;


  typedef SimpleDataObjectDecorator< RegionType > RegionObjectType;

  // Change the acces from protected to public
  using Superclass::SetNumberOfStreamDivisions;
  using Superclass::GetNumberOfStreamDivisions;


  RegionObjectType *GetRegionOutput()
    { return static_cast< RegionObjectType * >( this->ProcessObject::GetOutput(0) ); }
  const RegionObjectType *GetRegionOutput() const
    { return static_cast< const RegionObjectType * >( this->ProcessObject::GetOutput(0) ); }

  RegionType GetRegion(void) const
    { return this->GetRegionOutput()->Get(); }

  static RegionType RegionUnion( const RegionType &r1, const RegionType &r2 )
    {
      RegionType r;
      if ( r1.GetNumberOfPixels() == 0 )
        {
        if ( r2.GetNumberOfPixels() == 0 )
          {
          return r;
          }
        else
          {
          return r2;
          }
        }
      else if ( r2.GetNumberOfPixels() == 0 )
        {
        return r1;
        }
      typename InputImageType::IndexType lower, upper;
      for ( unsigned int i = 0; i < InputImageType::ImageDimension; ++i )
        {
        lower[i] = std::min( r1.GetIndex()[i], r2.GetIndex()[i] );
        upper[i] = std::max( r1.GetUpperIndex()[i], r2.GetUpperIndex()[i] );
        }
      r.SetIndex(lower);
      r.SetUpperIndex(upper);
      return r;
    }

  using Superclass::MakeOutput;
  DataObject::Pointer MakeOutput(typename Superclass::DataObjectPointerArraySizeType  output) override
    {
      switch ( output )
        {
        case 0:
          return RegionObjectType::New().GetPointer();
          break;
        default:
          return nullptr;
        }
    }

protected:
  BoundingRegionImageSinc()
    {
      this->ProcessObject::SetNumberOfRequiredOutputs(1);
      this->ProcessObject::SetNthOutput( 0, this->MakeOutput(0).GetPointer() );
    }
  ~BoundingRegionImageSinc() {}

  void PrintSelf(std::ostream & os, Indent indent) const  ITK_OVERRIDE
    {
      Superclass::PrintSelf(os, indent);

      os << indent << "Region: " << this->GetRegion() << std::endl;
    }


  void ThreadedStreamedGenerateData(const RegionType &inputRegionForChunk) override
    {

      typedef ImageScanlineConstIterator< TInputImage > InputConstIteratorType;

      const typename RegionType::SizeType &regionSize = inputRegionForChunk.GetSize();
      const size_t numberOfLinesToProcess = std::accumulate( regionSize.GetSize()+1,
                                                             regionSize.GetSize()+TInputImage::ImageDimension,
                                                             size_t(1),
                                                             std::multiplies<size_t>() );


      const InputImageType *inputPtr = this->GetInput();

      InputConstIteratorType inputIt(inputPtr, inputRegionForChunk);

      typename InputImageType::IndexType lower, upper;
      for ( unsigned int i = 0; i < InputImageType::ImageDimension; ++i )
        {
        lower[i] = NumericTraits< IndexValueType >::max();
        upper[i] = NumericTraits< IndexValueType >::NonpositiveMin();
        }

      inputIt.GoToBegin();
      while ( !inputIt.IsAtEnd() )
        {
        typename InputImageType::IndexType index = inputIt.GetIndex();
        // scan for start
        while ( !inputIt.IsAtEndOfLine() )
          {
          if ( inputIt.Get() )
            {
            index = inputIt.GetIndex();
            for( unsigned int i = 0; i < InputImageType::ImageDimension; ++i )
              {
              lower[i] = std::min(lower[i], index[i]);
              upper[i] = std::max(upper[i], index[i]);
              }
            break;
            }
          ++inputIt;
          }

        // scan for end
        while ( !inputIt.IsAtEndOfLine() )
          {
          if ( inputIt.Get() )
            {
            index = inputIt.GetIndex();
            }
          ++inputIt;
          }

        if ( inputPtr->GetPixel(index) )
          {
          for( unsigned int i = 0; i < InputImageType::ImageDimension; ++i )
            {
            lower[i] = std::min(lower[i], index[i]);
            upper[i] = std::max(upper[i], index[i]);
            }
          }

        inputIt.NextLine();
        }

      RegionType r;
      r.SetIndex(lower);
      for ( unsigned int i = 0; i < InputImageType::ImageDimension; ++i )
        {
        if (lower[i] <= upper[i])
          {
          r.SetSize(i, upper[i]-lower[i]+1);
          }
        else
          {
          r.SetSize(i,0);
          }
        }


      std::lock_guard<std::mutex> mutexHolder(m_Mutex);
      m_ThreadRegion = RegionUnion(m_ThreadRegion, r);
    }

  void AfterStreamedGenerateData( void ) override
    {
      this->GetRegionOutput()->Set(m_ThreadRegion);
}


private:
  ITK_DISALLOW_COPY_AND_ASSIGN(BoundingRegionImageSinc);

  RegionType m_ThreadRegion;


  std::mutex m_Mutex;
};

} // end namespace itk

#endif //itkBoundingRegionImageSinc_h
