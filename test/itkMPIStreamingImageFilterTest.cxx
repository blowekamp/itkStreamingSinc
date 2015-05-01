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


#include "itkMPIStreamingImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

int itkMPIStreamingImageFilterTest( int argc, char *argv[] )
{

  MPI_Init( &argc, &argv );

  typedef itk::Image< float, 3 > ImageType;

  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  typedef itk::Local::MPIStreamingImageFilter<ImageType> MPIStreamerType;
  MPIStreamerType::Pointer streamer = MPIStreamerType::New();
  streamer->SetInput( reader->GetOutput() );

  int rank;
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );

  std::ostringstream ss;
  ss << "mpi" << rank << ".mha";

  typedef itk::ImageFileWriter< ImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( ss.str().c_str() );
  writer->SetInput( streamer->GetOutput() );
  writer->SetNumberOfStreamDivisions( 2 );
  writer->Update();


  MPI_Finalize();

  return 0;
}
