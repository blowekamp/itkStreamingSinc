#
# Find the packages required by this module
#


# Find MPI configuration
INCLUDE( FindMPI )
OPTION( ITK_USE_MPI "Enables commands which utilize MPI" ${MPI_CXX_FOUND} )
