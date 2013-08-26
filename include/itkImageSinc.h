#ifndef __itkImageSink_h
#define __itkImageSink_h

#include "itkStreamingProcessObject.h"
#include "itkImage.h"
#include "itkImageRegionSplitterBase.h"
#include "itkImageRegionSplitterSlowDimension.h"

namespace itk
{

/** \class ImageSink
*
*
**/
template <class TInputImage >
class ImageSinc
  : public StreamingProcessObject
{
public:
  /** Standard class typedefs. */
  typedef ImageSinc                                      Self;
  typedef ProcessObject                                  Superclass;
  typedef SmartPointer<Self>                             Pointer;
  typedef SmartPointer<const Self>                       ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( ImageSinc, StreamingProcessObject );

  /** Smart Pointer type to a DataObject. */
  typedef DataObject::Pointer DataObjectPointer;

  /** Some convenient typedefs. */
  typedef TInputImage                         InputImageType;
  typedef typename InputImageType::Pointer    InputImagePointer;
  typedef typename InputImageType::RegionType InputImageRegionType;
  typedef typename InputImageType::PixelType  InputImagePixelType;

  /** SmartPointer to a region splitting object */
  typedef ImageRegionSplitterBase        SplitterType;
  typedef typename SplitterType::Pointer RegionSplitterPointer;

  /** Dimension of input images. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      InputImageType::ImageDimension);



  /** Set/Get the image input of this process object.  */
  virtual void SetInput(const InputImageType *input)
  {
    // Process object is not const-correct so the const_cast is required here
    this->ProcessObject::SetNthInput( 0, const_cast< InputImageType * >( input ) );
  }
  const InputImageType * GetInput(void) const
  {
    if ( this->GetNumberOfInputs() < 1 )
      {
      return 0;
      }

    return static_cast< const TInputImage * >
      ( this->ProcessObject::GetInput(0) );
  }
  const InputImageType *GetInput(unsigned int idx) const
  {
    return static_cast< const TInputImage * >
      ( this->ProcessObject::GetInput(idx) );
  }

  virtual void Update( )
  {
    this->UpdateOutputInformation();
    // if output 1, the just call it
    // this->PropagateRequestedRegion( NULL );
    this->UpdateOutputData( NULL );
  }

protected:
  ImageSinc()
    {
      // create default region splitter
      m_RegionSplitter = ImageRegionSplitterSlowDimension::New();
      m_NumberOfStreamDivisions = 1;
    }
  // ~ImageSinc() {} use default virtual implementation

  virtual void PrintSelf(std::ostream & os, Indent indent) const
  {
    Superclass::PrintSelf( os, indent );
    os << indent << "NumberOfStreamDivisions: " << this->m_NumberOfStreamDivisions << std::endl;
    os << indent << "RegionSplitter: " << this->m_RegionSplitter << std::endl;
  }


  virtual unsigned int 	GetNumberOfInputRequestedRegions (void)
  {
    const InputImageType* inputPtr = const_cast< InputImageType * >( this->GetInput() );
    InputImageRegionType inputImageRegion = inputPtr->GetLargestPossibleRegion();

    return this->GetRegionSplitter()->GetNumberOfSplits( inputImageRegion, this->m_NumberOfStreamDivisions );
  }

  virtual void 	GenerateNthInputRequestedRegion (unsigned int inputRequestedRegionNumber)
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


  virtual void StreamedGenerateData( unsigned int  inputRequestedRegionNumber)
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

  virtual void ThreadedStreamedGenerateData( const InputImageRegionType &inputRegionForChunk, ThreadIdType ) = 0;


  /** Set the number of pieces to divide the input.  The upstream pipeline
   * will be executed this many times. */
  itkSetMacro(NumberOfStreamDivisions, unsigned int);

  /** Get the number of pieces to divide the input. The upstream pipeline
    * will be executed this many times. */
  itkGetConstReferenceMacro(NumberOfStreamDivisions, unsigned int);

  /** Set/Get the helper class for dividing the input into chunks. */
  itkSetObjectMacro(RegionSplitter, SplitterType);
  itkGetObjectMacro(RegionSplitter, SplitterType);

  /** Static function used as a "callback" by the MultiThreader.  The threading
   * library will call this routine for each thread, which will delegate the
   * control to ThreadedGenerateData(). */
  static ITK_THREAD_RETURN_TYPE ThreaderCallback(void *arg)
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

   /** Internal structure used for passing image data into the threading library
    */
   struct ThreadStruct {
     Pointer Filter;
     InputImageRegionType currentInputRegion;
  };

private:
  ImageSinc(const ImageSinc &); // purposely not implemented
  ImageSinc &operator=(const ImageSinc &); // purposely not implemented

  unsigned int m_NumberOfStreamDivisions;
  RegionSplitterPointer m_RegionSplitter;
  InputImageRegionType m_CurrentInputRegion;
};

}

#endif // __itkImageSinc_h
