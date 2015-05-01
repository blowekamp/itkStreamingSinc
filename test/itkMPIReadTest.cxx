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

#include <mpi.h>
#include <iostream>
#include "itkMPIStreamingImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTimeProbe.h"


int itkMPIReadTest( int argc, char *argv[] )
{
  MPI_Init( &argc, &argv );

  int size;
  int rank;
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &size );

  bool distributedRead = true;

  if ( argc < 3 )
    {
    std::cerr << "Usage: " << argv[0] << " inFilename outFilename [distributeReadBool=1]" << std::endl;
    return EXIT_FAILURE;
    }

  if ( argc > 3 )
    {
    distributedRead = ( atoi(argv[2]) != 0 );
    }

  // only works with float
  typedef itk::Image< float, 3 > ImageType;

  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );


  typedef itk::MPIStreamingImageFilter<ImageType> MPIStreamerType;
  MPIStreamerType::Pointer streamer = MPIStreamerType::New();
  streamer->SetInput( reader->GetOutput() );

  if ( !distributedRead )
    {
    itk::TimeProbe t1;
    t1.Start();
    reader->UpdateLargestPossibleRegion();
    t1.Stop();

    if ( rank == 0 )
      {
      std::cout << "All read processed in " <<  t1.GetTotal() << t1.GetUnit() << std::endl;
      }
    reader->GetOutput()->DisconnectPipeline();
    }

  itk::TimeProbe t2;
  t2.Start();
  streamer->UpdateLargestPossibleRegion();
  t2.Stop();


  if ( rank == 0 )
    {
    std::cout << "Distributed with " << size << " processes in " <<  t2.GetTotal() << t2.GetUnit() << std::endl;

    typedef itk::ImageFileWriter< ImageType > WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( argv[2] );
    writer->SetInput(streamer->GetOutput());
    streamer->GetOutput()->DisconnectPipeline(); // IMPORTANT to
                                                 // disconnect from
                                                 // MPI pipeline!!! 
    writer->Update();
    }


  MPI_Finalize();

  return 0;
}
