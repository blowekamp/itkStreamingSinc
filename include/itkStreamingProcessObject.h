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
#ifndef itkStreamingProcessObject_h
#define itkStreamingProcessObject_h

#include "itkProcessObject.h"

namespace itk
{

/** \class Add the methods needed to generically iterate on the input
 * requested region to stream data.
 *
 * \ingroup StreamingSinc
 */
class StreamingProcessObject
  : public ProcessObject
{
public:
  /** Standard class typedefs. */
  typedef StreamingProcessObject                         Self;
  typedef ProcessObject                                  Superclass;
  typedef SmartPointer<Self>                             Pointer;
  typedef SmartPointer<const Self>                       ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(StreamingProcessObject,ProcessObject);


  virtual void PropagateRequestedRegion(DataObject *output) ITK_OVERRIDE;

  virtual void GenerateData( void ) ITK_OVERRIDE;

  /** Override UpdateOutputData() from ProcessObject to divide upstream
   * updates into pieces. This filter does not have a GenerateData()
   * or ThreadedGenerateData() method.  Instead, all the work is done
   * in UpdateOutputData() since it must update a little, execute a little,
   * update some more, execute some more, etc. */
  virtual void UpdateOutputData(DataObject *output) ITK_OVERRIDE;

  /** the current request number of -1, it not currently streaming */
  virtual int GetCurrentRequestNumber( ) const { return m_CurrentRequestNumber; }

  virtual void ResetPipeline()  ITK_OVERRIDE { Superclass::ResetPipeline(); m_CurrentRequestNumber = -1; }

protected:
  StreamingProcessObject( void ) { m_CurrentRequestNumber = -1; }
  void PrintSelf(std::ostream& os, Indent indent) const  ITK_OVERRIDE { Superclass::PrintSelf(os,indent); }


  virtual unsigned int GetNumberOfInputRequestedRegions( void ) = 0;
  virtual void GenerateNthInputRequestedRegion( unsigned int inputRequestedRegionNumber ) = 0;

  virtual void StreamedGenerateData( unsigned int inputRequestedRegionNumber ) = 0;
  virtual void BeforeStreamedGenerateData( void ) {};
  virtual void AfterStreamedGenerateData( void ) {};

private:
  ITK_DISALLOW_COPY_AND_ASSIGN(StreamingProcessObject);

  int m_CurrentRequestNumber;
};

} // end namespace itk

#endif //itkStreamingProcessObject_h
