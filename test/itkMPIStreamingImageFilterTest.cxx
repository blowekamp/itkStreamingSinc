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
