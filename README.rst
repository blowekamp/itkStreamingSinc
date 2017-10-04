Streaming Sinc Filter Collection
================================


General
-------

This repository contains an `ITK: The Insight Toolkit for Segmentation and Registration <https://itk.org>`_ module
with advanced classes for image streaming and MPI distributed image processing through streaming.  It is designed
to work with the ITKv4 modular system by being placed in the ITK source code.

Getting Started
---------------

This module should be cloned into a checkout of the ITK reposiory as a subdirectory
in the "Modules/External".

The following is a brief list of instructions to get a external module
into ITK::

  cd ITK/Modules/External/
  git clone https://github.com/blowekamp/itkStreamingSinc.git

Then configure ITK, set::

  Module_StreamingSinc:BOOL=ON

in ITK's CMake build configuration. This external module will need to be manually
updated from the respository.


Author
------

Bradley Lowekamp
