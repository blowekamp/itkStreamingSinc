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
#ifndef itkImageSinc_hxx
#define itkImageSinc_hxx

#include "itkImageSinc.h"

namespace itk
{

template <class TInputImage>
ImageSinc<TInputImage>
::ImageSinc()
{
  // create default region splitter
  m_RegionSplitter = ImageRegionSplitterSlowDimension::New();
  m_NumberOfStreamDivisions = 1;
}

template <class TInputImage>
void
ImageSinc<TInputImage>
::SetInput(const InputImageType *input)
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput( 0, const_cast< InputImageType * >( input ) );
}

template <class TInputImage>
const typename ImageSinc<TInputImage>::InputImageType *
ImageSinc<TInputImage>
::GetInput(void) const
{
  if ( this->GetNumberOfInputs() < 1 )
    {
    return 0;
    }

  return static_cast< const TInputImage * >
    ( this->ProcessObject::GetInput(0) );
}

template <class TInputImage>
const typename ImageSinc<TInputImage>::InputImageType *
ImageSinc<TInputImage>
::GetInput(unsigned int idx) const
{
  return static_cast< const TInputImage * >
    ( this->ProcessObject::GetInput(idx) );
}

template <class TInputImage>
void
ImageSinc<TInputImage>
::Update( )
{
  this->UpdateOutputInformation();
  // if output 1, the just call it
  // this->PropagateRequestedRegion( NULL );
  this->UpdateOutputData( NULL );
}

template <class TInputImage>
void
ImageSinc<TInputImage>
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "NumberOfStreamDivisions: " << this->m_NumberOfStreamDivisions << std::endl;
  os << indent << "RegionSplitter: " << this->m_RegionSplitter << std::endl;
}

template <class TInputImage>
unsigned int
ImageSinc<TInputImage>
::GetNumberOfInputRequestedRegions (void)
{
  const InputImageType* inputPtr = const_cast< InputImageType * >( this->GetInput() );
  InputImageRegionType inputImageRegion = inputPtr->GetLargestPossibleRegion();

  return this->GetRegionSplitter()->GetNumberOfSplits( inputImageRegion, this->m_NumberOfStreamDivisions );
}

template <class TInputImage>
void
ImageSinc<TInputImage>
::GenerateNthInputRequestedRegion (unsigned int inputRequestedRegionNumber)
{
  Superclass::GenerateInputRequestedRegion();

  InputImageType* inputPtr = const_cast< InputImageType * >( this->GetInput() );
  InputImageRegionType inputImageRegion = inputPtr->GetLargestPossibleRegion();


  this->GetRegionSplitter()->GetSplit( inputRequestedRegionNumber,
                                       this->GetNumberOfInputRequestedRegions( ),
                                       inputImageRegion );
  m_CurrentInputRegion = inputImageRegion;

  itkDebugMacro( "Generating " << inputRequestedRegionNumber << " chunk as " <<  m_CurrentInputRegion );

  for ( unsigned int idx = 0; idx < this->GetNumberOfInputs(); ++idx )
    {
    if ( this->GetInput(idx) )
      {
      // Check whether the input is an image of the appropriate
      // dimension (use ProcessObject's version of the GetInput()
      // method since it returns the input as a pointer to a
      // DataObject as opposed to the subclass version which
      // static_casts the input to an TInputImage).
      typedef ImageBase< InputImageDimension > ImageBaseType;
      typename ImageBaseType::ConstPointer constInput =
        dynamic_cast< ImageBaseType const * >( this->ProcessObject::GetInput(idx) );

      // If not an image, skip it, and let a subclass of
      // ImageToImageFilter handle this input.
      if ( constInput.IsNull() )
        {
        continue;
        }

      // Input is an image, cast away the constness so we can set
      // the requested region.
      InputImagePointer input =
        const_cast< TInputImage * >( this->GetInput(idx) );

      // copy the requested region of the first input to the others
      InputImageRegionType inputRegion;
      input->SetRequestedRegion( m_CurrentInputRegion );
      }
    }
}


template <class TInputImage>
void
ImageSinc<TInputImage>
::StreamedGenerateData( unsigned int itkNotUsed(inputRequestedRegionNumber) )
{

  this->GetMultiThreader()->SetNumberOfWorkUnits( this->GetNumberOfWorkUnits() );
  const ThreadIdType  total = this->GetRegionSplitter()->GetNumberOfSplits( m_CurrentInputRegion,  this->GetNumberOfWorkUnits() );

  this->GetMultiThreader()->ParallelizeArray(
    0,
    total,
    [this, total](SizeValueType threadId)
    {
        InputImageRegionType splitRegion = this->m_CurrentInputRegion;
        this->GetRegionSplitter()->GetSplit( threadId, total, splitRegion );
        this->ThreadedStreamedGenerateData(splitRegion, threadId);
    },
    nullptr); // \Modules\Core\Common\include\itkProgressTransformer.h would need to be used to properly report progress here
}

}

#endif // itkImageSinc_hxx
