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
#ifndef itkImageSinc_h
#define itkImageSinc_h

#include "itkStreamingProcessObject.h"
#include "itkImage.h"
#include "itkImageRegionSplitterBase.h"
#include "itkImageRegionSplitterSlowDimension.h"

namespace itk
{

/** \class ImageSink
*
* \ingroup StreamingSinc
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


  using Superclass::SetInput;
  /** Set/Get the image input of this process object.  */
  virtual void SetInput(const InputImageType *input);

  virtual const InputImageType * GetInput(void) const;

  virtual const InputImageType *GetInput(unsigned int idx) const;

  virtual void Update( ) ITK_OVERRIDE;

protected:
  ImageSinc();

  virtual void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;

  virtual unsigned int GetNumberOfInputRequestedRegions (void) ITK_OVERRIDE;

  virtual void  GenerateNthInputRequestedRegion (unsigned int inputRequestedRegionNumber) ITK_OVERRIDE;

  virtual void StreamedGenerateData( unsigned int  inputRequestedRegionNumber) ITK_OVERRIDE;

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
  static ITK_THREAD_RETURN_TYPE ThreaderCallback(void *arg);

   /** Internal structure used for passing image data into the threading library
    */
   struct ThreadStruct {
     Pointer Filter;
     InputImageRegionType currentInputRegion;
  };

private:
  ITK_DISALLOW_COPY_AND_ASSIGN(ImageSinc);

  unsigned int          m_NumberOfStreamDivisions;
  RegionSplitterPointer m_RegionSplitter;
  InputImageRegionType  m_CurrentInputRegion;
};

}

#include "itkImageSinc.hxx"

#endif // itkImageSinc_h
