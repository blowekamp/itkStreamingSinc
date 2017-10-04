This ITK module contains some advanced classes for image streaming and

MPI distributed processing through streaming. 


General
-------

This is a module for ITK: The Insight Toolkit for Segmentation and
Registration. It is designed to work with the ITKv4 modular system and
to be placed in ITK/Modules/External.


Getting Started
---------------

This module should be cloned into the ITK reposiory as a subdirectory
in the "Modules/External" directory.

The following is a brief list of instructions to get a external module
into ITK:

cd ITK/Modules/External/
git clone https://github.com/blowekamp/itkStreamingSinc.git

Then configure ITK as not make but set "Module_StreamingSinc" to "ON" to
enable this module. The external module will need to be manually
updated from the git respository.


Author
------

Bradley Lowekamp
