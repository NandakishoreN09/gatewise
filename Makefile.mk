#****************************************************************
#Usage:

# Automatically compile source files in this directory.
# To include additional directories, use SRC_SEARCH_DIR += <directory_path>
# Example: SRC_SEARCH_DIR += ~/Desktop/example/

# To include specific files from other directories:
# For .c files: C_SRC += <full_path>
# For .cpp files: CPP_SRC += <full_path>
# For .ino files: INO_SRC += <full_path>
# For .pde files: PDE_SRC += <full_path>

# Optional compile-time flags:
# Use CFLAGS += or CPPFLAGS += <flags>
#****************************************************************

# Include the library folder
SRC_SEARCH_DIR += ./library/

# Explicitly add parking_system.cpp
CPP_SRC += library/parking_system.cpp

# Add include paths for library headers
CPPFLAGS += -I./library

# Flags for additional debugging
CFLAGS += -g
CPPFLAGS += -g


