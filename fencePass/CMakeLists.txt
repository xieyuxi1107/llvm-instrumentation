add_llvm_library(LLVMFENCE MODULE            # Name of the shared library that you will generate
       fencePass.cpp                          # Your pass
       PLUGIN_TOOL
       opt
)
