This repository contains the cleaned up verion of the slt tools.

N.B. On github, this README might be referencing things that are not present in 
the repository yet. This is normal for the time being, as we gradually 
roll out the code.

# Using the toolset.

slt_tech is not meant to be included as a subproject. This is because there is a
number of executable tools that are built as part of it that are used when
compiling an app making use of the toolset, and the build process would become
needlessly convoluted.

Instead, the recommended workflow is to install all three configurations into
the same destination directory, and use that as SLT_BUILT_DIR
(see sample project).

## Third party dependencies

As much as possible, dependencies are set up as git submodules. However, 
slt_tech depends on and the FBX SDK, which needs to be provided.

## Caveats
On paper, this is meant to be platform agnostic, but in practice, the CMake
configuration has only been tested on windows for the time being

# Contributor's guide
## Code organization

- Source files, tests and internal headers go in the src/slt directory
- All headers meant to be copied into the toolset's release belong in the 
  include/slt directory
- Headers in the include/slt directory are meant to be read as documentation
- If modules need to expose implementations or declare types/functions that
  are not relevant to a user of the toolset, these declarations/definitions
  must go in the module/details directory, and may not share the same namespace
  as user-facing constructs.
- Avoid having headers with identical file names within the same module, it 
  confuses the MSVC project.
