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
  // Set up the multithreaded processing
  ThreadStruct str;
  str.Filter = this;
  str.currentInputRegion = m_CurrentInputRegion;

  this->GetMultiThreader()->SetNumberOfThreads( this->GetNumberOfThreads() );
  this->GetMultiThreader()->SetSingleMethod(this->ThreaderCallback, &str);

  // multithread the execution
  this->GetMultiThreader()->SingleMethodExecute();

}


template <class TInputImage>
ITK_THREAD_RETURN_TYPE
ImageSinc<TInputImage>
::ThreaderCallback(void *arg)
{
  ThreadStruct *str;

  const ThreadIdType threadId = ( (MultiThreader::ThreadInfoStruct *)( arg ) )->ThreadID;
  const ThreadIdType  threadCount = ( (MultiThreader::ThreadInfoStruct *)( arg ) )->NumberOfThreads;

  str = (ThreadStruct *)( ( (MultiThreader::ThreadInfoStruct *)( arg ) )->UserData );

  const ThreadIdType  total = str->Filter->GetRegionSplitter()->GetNumberOfSplits( str->currentInputRegion,  threadCount );

  if ( threadId < total )
    {
    // execute the actual method with appropriate region
    // first find out how many pieces extent can be split into.
    InputImageRegionType splitRegion = str->currentInputRegion;

    str->Filter->GetRegionSplitter()->GetSplit( threadId, total, splitRegion );

    str->Filter->ThreadedStreamedGenerateData(splitRegion, threadId);
    }
  // else
  //   {
  //   otherwise don't use this thread. Sometimes the threads dont
  //   break up very well and it is just as efficient to leave a
  //   few threads idle.
  //   }

  return ITK_THREAD_RETURN_VALUE;
}

}

#endif // itkImageSinc_hxx
