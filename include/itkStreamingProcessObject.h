
#ifndef __itkStreamingProcessObject_h
#define __itkStreamingProcessObject_h

#include "itkProcessObject.h"

namespace itk
{

/** \class Add the methods needed to generically iterate on the input
 * requested region to stream data.
 *
 * \todo document me
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


  virtual void PropagateRequestedRegion(DataObject *output);

  virtual void GenerateData( void );

  /** Override UpdateOutputData() from ProcessObject to divide upstream
   * updates into pieces. This filter does not have a GenerateData()
   * or ThreadedGenerateData() method.  Instead, all the work is done
   * in UpdateOutputData() since it must update a little, execute a little,
   * update some more, execute some more, etc. */
  virtual void UpdateOutputData(DataObject *output);

  /** the current request number of -1, it not currently streaming */
  virtual int GetCurrentRequestNumber( ) const { return m_CurrentRequestNumber; }

  virtual void ResetPipeline() { Superclass::ResetPipeline(); m_CurrentRequestNumber = -1; }

protected:
  StreamingProcessObject( void ) { m_CurrentRequestNumber = -1; }
  // ~StreamingProcessObject(); use default virtual implementation
  void PrintSelf(std::ostream& os, Indent indent) const { Superclass::PrintSelf(os,indent); }


  virtual unsigned int GetNumberOfInputRequestedRegions( void ) = 0;
  virtual void GenerateNthInputRequestedRegion( unsigned int inputRequestedRegionNumber ) = 0;

  virtual void StreamedGenerateData( unsigned int inputRequestedRegionNumber ) = 0;
  virtual void BeforeStreamedGenerateData( void ) {};
  virtual void AfterStreamedGenerateData( void ) {};

private:
  StreamingProcessObject(const StreamingProcessObject&); //purposely not implemented
  void operator=(const StreamingProcessObject&); //purposely not implemented

  int m_CurrentRequestNumber;
};

} // end namespace itk

#endif //__itkStreamingProcessObject_h
