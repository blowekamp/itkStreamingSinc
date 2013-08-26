#include "itkStreamingProcessObject.h"

namespace itk
{

void StreamingProcessObject::GenerateData( void )
{
  // todo add lock to this function

  this->BeforeStreamedGenerateData();


  //
  // Determine of number of pieces to divide the input.  This will be the
  // minimum of what the user specified via SetNumberOfStreamDivisions()
  // and what the Splitter thinks is a reasonable value.
  //
  unsigned int numberOfInputRequestRegion = this->GetNumberOfInputRequestedRegions();

  //
  // Loop over the number of pieces, execute the upstream pipeline on each
  // piece, and copy the results into the output image.
  //
  for (unsigned int piece = 0; piece < numberOfInputRequestRegion  && !this->GetAbortGenerateData();  piece++)
    {
    this->m_CurrentRequestNumber = piece;

    this->GenerateNthInputRequestedRegion( piece );

    //
    // Now that we know the input requested region, propagate this
    // through all the inputs.
    // ;
    DataObjectPointerArraySizeType idx;
    for (idx = 0; idx < this->GetNumberOfInputs(); ++idx)
      {
      if ( this->GetInput(idx) )
        {
        this->GetInput(idx)->PropagateRequestedRegion();
        }
      }

    //
    // Propagate the update call - make sure everything we
    // might rely on is up-to-date
    // Must call PropagateRequestedRegion before UpdateOutputData if multiple
    // inputs since they may lead back to the same data object.
    m_Updating = true;
    for (idx = 0; idx < this->GetNumberOfInputs(); ++idx)
      {
      if (this->GetInput(idx))
        {
        if ( idx != 0  && this->GetNumberOfInputs() > 1)
          {
          this->GetInput(idx)->PropagateRequestedRegion();
          }
        this->GetInput(idx)->UpdateOutputData();
        }
      }

    //
    try
      {
      this->StreamedGenerateData( piece );
      }
    catch( ProcessAborted & excp )
      {
      this->InvokeEvent( AbortEvent() );
      this->ResetPipeline();
      this->RestoreInputReleaseDataFlags();
      throw excp;
      }
    catch( ... )
      {
      this->ResetPipeline();
      this->RestoreInputReleaseDataFlags();
      throw;
      }
    }


  this->AfterStreamedGenerateData();
}

void StreamingProcessObject::UpdateOutputData(DataObject *output)
{

  unsigned int idx;

  //
  //prevent chasing our tail
  //
  if (this->m_Updating)
    {
    return;
    }


  //
  // Prepare all the outputs. This may deallocate previous bulk data.
  //
  this->PrepareOutputs();

  /**
   * Cache the state of any ReleaseDataFlag's on the inputs. While the
   * filter is executing, we need to set the ReleaseDataFlag's on the
   * inputs to false in case the current filter is implemented using a
   * mini-pipeline (which will try to release the inputs).  After the
   * filter finishes, we restore the state of the ReleaseDataFlag's
   * before the call to ReleaseInputs().
   */
  this->CacheInputReleaseDataFlags();

  /**
   * Make sure we have the necessary inputs
   */
  unsigned int ninputs = this->GetNumberOfValidRequiredInputs();
  if (ninputs < this->GetNumberOfRequiredInputs())
    {
    itkExceptionMacro(<< "At least " << static_cast<unsigned int>( this->GetNumberOfRequiredInputs() ) << " inputs are required but only " << ninputs << " are specified.");
    return;
    }

  this->SetAbortGenerateData( false );
  this->SetProgress(0.0);
  this->m_Updating = true;

  /**
   * Tell all Observers that the filter is starting
   */
  this->InvokeEvent( StartEvent() );

  this->Self::GenerateData( );
  /*
   * If we ended due to aborting, push the progress up to 1.0 (since
   * it probably didn't end there)
   */
  if ( this->GetAbortGenerateData() )
    {
    this->UpdateProgress(1.0);
    }

  // Notify end event observers
  this->InvokeEvent( EndEvent() );


  /**
   * Now we have to mark the data as up to data.
   */
  for (idx = 0; idx < this->GetNumberOfOutputs(); ++idx)
    {
    if (this->GetOutput(idx))
      {
      this->GetOutput(idx)->DataHasBeenGenerated();
      }
    }

  /**
   * Restore the state of any input ReleaseDataFlags
   */
  //this->RestoreInputReleaseDataFlags();

  /**
   * Release any inputs if marked for release
   */
  this->ReleaseInputs();

  // Mark that we are no longer updating the data in this filter
  this->m_Updating = false;

}

/** Override PropagateRequestedRegion from ProcessObject
 *  Sinke inside UpdateOutputData we iterate over streaming pieces
 *  we don't need to proapage up the pipeline
 */
void StreamingProcessObject::PropagateRequestedRegion(DataObject *output)
{

  /**
   * check flag to avoid executing forever if there is a loop
   */
  if (this->m_Updating)
    {
    return;
    }

  /**
   * Give the subclass a chance to indicate that it will provide
   * more data then required for the output. This can happen, for
   * example, when a source can only produce the whole output.
   * Although this is being called for a specific output, the source
   * may need to enlarge all outputs.
   */
  this->EnlargeOutputRequestedRegion( output );


  /**
   * Give the subclass a chance to define how to set the requested
   * regions for each of its outputs, given this output's requested
   * region.  The default implementation is to make all the output
   * requested regions the same.  A subclass may need to override this
   * method if each output is a different resolution.
   */
  this->GenerateOutputRequestedRegion( output );

  // we don't call GenerateInputRequestedRegion since the requested
  // regions are manage when the pipeline is execute

  // we don't call inputs PropagateRequestedRegion either
  // because the pipeline managed later
}

} // end namespace itk
