

#${itk-module} will be the name of this module and will not need to be
#changed when this module is renamed.

set(${itk-module}_SRC
  itkStreamingProcessObject.cxx
)

itk_module_add_library(${itk-module} ${${itk-module}_SRC})
