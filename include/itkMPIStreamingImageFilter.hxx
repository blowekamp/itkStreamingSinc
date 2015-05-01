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

#ifndef itkMPIStreamingImageFilter_hxx
#define itkMPIStreamingImageFilter_hxx

#include "itkMPIStreamingImageFilter.h"

namespace itk
{

/**
 *
 */
template < class TImageType >
MPIStreamingImageFilter< TImageType >
::MPIStreamingImageFilter()
{
  m_MPITAG = 99;

  // create default region splitter
  m_RegionSplitter = ImageRegionSplitterSlowDimension::New();
}


/**
 *
 */
template < class TImageType >
MPIStreamingImageFilter< TImageType >
::~MPIStreamingImageFilter()
{
}

/**
 *
 */
template < class TImageType >
void
MPIStreamingImageFilter< TImageType >
::UpdateOutputInformation()
{
  // perform the standard update output information
  Superclass::UpdateOutputInformation();

  // descide who are the processes envolved
  MPI_Comm_rank( MPI_COMM_WORLD, &m_MPIRank );
  MPI_Comm_size( MPI_COMM_WORLD, &m_MPISize );
}


/**
 *
 */
template < class TImageType >
void
MPIStreamingImageFilter< TImageType >
::GenerateOutputRequestedRegion(DataObject *outputDO)
{
  // perform the standard propagation to all outputs
  Superclass::GenerateOutputRequestedRegion( outputDO );

  // find the index for this output
  unsigned int outputNumber = outputDO->GetSourceOutputIndex();

  ImageType* output = this->GetOutput( outputNumber );

  if ( output )
    {

    this->m_MPIOutputRegions.resize( m_MPISize );

    // share the output requested region will all processes
    for ( int rank = 0; rank < m_MPISize; ++rank )
      {
      RegionType r = output->GetRequestedRegion();

      typename RegionType::IndexValueType indexValue;
      typename RegionType::SizeValueType sizeValue;

      for ( unsigned int j = 0; j < ImageType::ImageDimension; ++j )
        {

        if ( rank == m_MPIRank )
          {
          indexValue = r.GetIndex( j );
          sizeValue = r.GetSize( j );
          }

        MPI_Bcast( &indexValue, 1, MPI_LONG, rank, MPI_COMM_WORLD );
        MPI_Bcast( &sizeValue, 1, MPI_UNSIGNED_LONG, rank, MPI_COMM_WORLD );

        if ( rank != m_MPIRank )
          {
          r.SetIndex( j, indexValue );
          r.SetSize( j, sizeValue );
          }
        }

      if (m_MPIRank == 0 )
        {
        itkDebugMacro( << "RANK " << rank << " output region : " << r );
        }
      this->m_MPIOutputRegions[rank] = r;

      }
    }
}


/**
 *
 */
template < class TImageType >
void
MPIStreamingImageFilter< TImageType >
::GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();

  // Intersect all ouput requested regions
  RegionType outputRegion = m_MPIOutputRegions[0];
  for ( int i = 1; i < m_MPISize; ++i )
    {
    for ( unsigned int j = 0; j < ImageType::ImageDimension; ++j )
      {
      // choose min
      if ( outputRegion.GetIndex( j ) >  m_MPIOutputRegions[i].GetIndex( j ) )
        {
        typename RegionType::OffsetValueType off = outputRegion.GetIndex( j ) - m_MPIOutputRegions[i].GetIndex( j );
        outputRegion.SetIndex( j,  m_MPIOutputRegions[i].GetIndex( j ) );
        outputRegion.SetSize( j,  outputRegion.GetSize( j ) + off );
        }

      // choose max
      if ( outputRegion.GetSize( j ) +  outputRegion.GetIndex( j )
           < m_MPIOutputRegions[i].GetIndex( j ) + m_MPIOutputRegions[i].GetSize( j ) )
        {
        outputRegion.SetSize( j, m_MPIOutputRegions[i].GetIndex( j ) + m_MPIOutputRegions[i].GetSize( j ) );
        }

      }
    }

  // split RR
  // Region Splitter
  unsigned int numSplits = m_RegionSplitter->GetNumberOfSplits(outputRegion, m_MPISize);
  typedef itk::ImageRegionSplitterSlowDimension SplitterType;
  typename SplitterType::Pointer splitter = SplitterType::New();

  // Get The splits
  m_MPIInputRegions.resize( m_MPISize );
  for ( int split = 0; split < m_MPISize; ++split )
    {
    if ( split < numSplits )
      {
      m_MPIInputRegions[ split ] = outputRegion;
      m_RegionSplitter->GetSplit( split, numSplits, m_MPIInputRegions[ split ] );
      }
    else
      {
      typename ImageType::SizeType  s;
      s.Fill( 0 );
      m_MPIInputRegions[ split ].SetSize( s );
      }
    }


  // set input to the split region
  ImageType* input = const_cast<ImageType *> (this->GetInput());
  if ( !input )
    {
    return;
    }

  itkDebugMacro( "RANK " << m_MPIRank << " requested region: " << m_MPIInputRegions[ m_MPIRank ] );

  input->SetRequestedRegion( m_MPIInputRegions[ m_MPIRank ] );
}


/**
 *
 */
template < class TImageType >
void
MPIStreamingImageFilter< TImageType >
::GenerateData()
{
  this->AllocateOutputs();

  const ImageType *  input = this->GetInput();

  // compute regions to send
  // this is the intersection of out input region with each output
  // region
  std::vector< RegionType > sendRegions( m_MPISize );
  std::vector< typename ImageType::Pointer > sendImages( m_MPISize );
  for ( int split = 0; split < m_MPISize; ++split )
    {
    sendRegions[ split ] = m_MPIInputRegions[ m_MPIRank ];
    bool good_crop = sendRegions[ split ].Crop( m_MPIOutputRegions[ split ] );

    // We don't need to talk to ourself.
    if (( split == m_MPIRank ) || ( !good_crop ))
      {
      typename ImageType::SizeType  s;
      s.Fill( 0 );
      sendRegions[ split ].SetSize( s );
      }

    typedef itk::ExtractImageFilter< ImageType, ImageType > ExtractorType;
    if ( sendRegions[ split ].GetNumberOfPixels() != 0 )
      {

      // check if we can reuse an extracted image
      for ( int i = 0; i < split; ++i )
        {
        if ( sendImages[i] && sendImages[i]->GetLargestPossibleRegion() == sendRegions[ split ] )
          {
          sendImages[split] = sendImages[i];
          }
        }

      // if we didn't reuse the image, then extract it
      if ( !sendImages[split] )
        {
        typename ExtractorType::Pointer extractor = ExtractorType::New();
        extractor->SetInput( input );
        extractor->SetExtractionRegion( sendRegions[ split ] );
        extractor->SetDirectionCollapseToIdentity();
        try
          {
          extractor->Update();
          }
        catch ( itk::ExceptionObject  & e )
          {
          std::cerr << "RANK " << m_MPIRank << std::endl;
          std::cerr << "Excpetion caught while exracting send region!" << std::endl;
          std::cerr << e << std::endl;
          }

        sendImages[split] = extractor->GetOutput();
        }
      }
    } // end for split

  //  check if all send Images are the same and not null, except the
  //  rank's send image
  int useBcastLocal = sendImages[(m_MPIRank+1)%m_MPISize].IsNotNull();
  for ( int split = 0; split < m_MPISize-1; ++split )
    {
    if ( split == m_MPIRank )
      continue;

    int nextSplit = split + 1;

    if ( m_MPIRank == nextSplit )
      {
      nextSplit = ( nextSplit + 1 ) % m_MPISize;
      }

    useBcastLocal &= ( sendImages[split] == sendImages[nextSplit] );
    }

  std::vector< int > useBcast( m_MPISize );
  MPI_Allgather( &useBcastLocal, 1, MPI_INT, &(useBcast[0]), 1, MPI_INT, MPI_COMM_WORLD );


  std::vector< RegionType > recvRegions( m_MPISize );
  std::vector< typename ImageType::Pointer > recvImages( m_MPISize );
  for ( int split = 0; split < m_MPISize; ++split )
    {
    recvRegions[ split ] = m_MPIOutputRegions[ m_MPIRank ];
    bool good_crop = recvRegions[ split ].Crop( m_MPIInputRegions[ split ] );

    // We don't need to talk to ourself.
    if (( split == m_MPIRank ) || ( !good_crop ))
      {
      typename ImageType::SizeType  s;
      s.Fill( 0 );
      recvRegions[ split ].SetSize( s );
      }

    if ( recvRegions[ split ].GetNumberOfPixels() != 0 )
      {
      recvImages[ split ] = ImageType::New();

      // set the information after the regions to that the
      // largest possible will be the same as the input
      recvImages[ split ]->SetRegions( recvRegions[ split ] );
      recvImages[ split ]->CopyInformation( input );
      recvImages[ split ]->Allocate();
      }
    }  // end for split


  // MPI Bcasts
  for ( int split = 0; split < m_MPISize; ++split )
    {
    if ( useBcast[m_MPIRank] )
      {
      if ( m_MPIRank == 0 )
        {
        itkDebugMacro( << "--Bcast--" );
        itkDebugMacro( << recvRegions[ split ] );
        }

      if ( split == m_MPIRank )
        {
        MPI_Bcast( sendImages[ (split+1)%m_MPISize ]->GetBufferPointer(),
                   sendRegions[ (split+1)%m_MPISize ].GetNumberOfPixels(),
                   MPI_FLOAT,
                   split,
                   MPI_COMM_WORLD );
        }
      else
        {
        MPI_Bcast( recvImages[ split ]->GetBufferPointer(),
                   recvRegions[ split ].GetNumberOfPixels(),
                   MPI_FLOAT,
                   split,
                   MPI_COMM_WORLD );
        }
      }
    }

  // MPI Send/Receive
  std::vector<MPI_Request> requests(m_MPISize*2);
  std::vector<MPI_Status> statuses(m_MPISize*2);
  size_t numberOfRequests = 0;
  for ( int split = 0; split < m_MPISize; ++split )
    {
    if ( !useBcast[split] )
      {
      if ( m_MPIRank == 0 )
        {
        itkDebugMacro( << "--SENDING--" );
        itkDebugMacro( << sendRegions[ split ] );
        itkDebugMacro( << "--RECEIVING--" );
        itkDebugMacro( << recvRegions[ split ] );
        }

      if ( recvRegions[ split ].GetNumberOfPixels() != 0 )
        {
        MPI_Irecv( recvImages[ split ]->GetBufferPointer(),
                   recvRegions[ split ].GetNumberOfPixels(),
                   MPI_FLOAT,
                   split,
                   m_MPITAG,
                   MPI_COMM_WORLD,
                   &requests[numberOfRequests++] );
        }
      if ( sendRegions[ split ].GetNumberOfPixels() != 0 &&  !useBcast[m_MPIRank] )
        {
        MPI_Isend( sendImages[ split ]->GetBufferPointer(),
                   sendRegions[ split ].GetNumberOfPixels(),
                   MPI_FLOAT,
                   split,
                   m_MPITAG,
                   MPI_COMM_WORLD,
                   &requests[numberOfRequests++] );
        }
      }

    } // end send/receive for split
  MPI_Waitall( numberOfRequests, &requests[0], &statuses[0] );

  typename ImageType::Pointer working = ImageType::New();
  working->Graft( this->GetOutput() );

  // copy input region to the working
  typedef itk::PasteImageFilter< ImageType, ImageType > PasteType;
  typename PasteType::Pointer inputPaste = PasteType::New();
  inputPaste->SetDestinationImage( working );
  inputPaste->SetDestinationIndex( m_MPIInputRegions[ m_MPIRank ].GetIndex() );
  inputPaste->SetSourceImage( input );
  inputPaste->SetSourceRegion( m_MPIInputRegions[ m_MPIRank ] );
  inputPaste->InPlaceOn();
  inputPaste->Update();

  working = inputPaste->GetOutput();

  // paste recved regions into outptut
  for ( int split = 0; split < m_MPISize; ++split )
    {
    if ( m_MPIRank == 0 )
      {
      itkDebugMacro( << "--PASTING--" );
      itkDebugMacro( << recvRegions[ split ] );
      itkDebugMacro( << working->GetLargestPossibleRegion() );
      }
    if ( recvRegions[ split ].GetNumberOfPixels() != 0 )
      {
      // Paste the receive image into working
      typename PasteType::Pointer paste = PasteType::New();
      paste->SetDestinationImage( working );
      paste->SetDestinationIndex( recvRegions[ split ].GetIndex() );
      paste->SetSourceImage( recvImages[ split ] );
      paste->SetSourceRegion( recvRegions[ split ] );
      paste->InPlaceOn();
      try
        {
        paste->Update();
        }
      catch( itk::ExceptionObject & excep )
        {
        std::cerr << "Exception caught while updating receive paste!" << std::endl;
        std::cerr << excep << std::endl;
        }
      working = paste->GetOutput();

      } // end use neighbors
    } // end use for split

  working->SetLargestPossibleRegion( input->GetLargestPossibleRegion() );
  this->GraftOutput( working );

}

/**
 *
 */
template < class TImageType >
void
MPIStreamingImageFilter< TImageType >
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "MPITAG: " << m_MPITAG << std::endl;
  os << indent << "MPIRank: " << m_MPIRank << std::endl;
  os << indent << "MPISize: " << m_MPISize << std::endl;

  const Indent indent2 = indent.GetNextIndent();
  os << indent << "MPIOutputRegions:" << std::endl;
  for (unsigned int i = 0; i < m_MPIOutputRegions.size(); ++i)
    {
    m_MPIOutputRegions[i].Print(os, indent2);
    }


  os << indent << "MPIInputRegions:" << std::endl;
  for (unsigned int i = 0; i < m_MPIInputRegions.size(); ++i)
    {
    m_MPIInputRegions[i].Print(os, indent2);
    }

  itkPrintSelfObjectMacro( RegionSplitter );

}


} // end namespace itk

#endif
