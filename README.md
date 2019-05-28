# Vrooom

To use OpenAL and Python with Visual Studio, do the following (replace (...) by the correct path): 

In project properties, 

-- put
***(...)***\OpenAL 1.1 SDK\include;%(AdditionalIncludeDirectories);***(...)***\Python\Python37-32\include
   in
C++ -> General -> Additional Include Directories

-- put 
***(...)***\OpenAL 1.1 SDK\libs\Win32;%(AdditionalLibraryDirectories)
   in
Linker -> General -> Additional Library Directories

-- put
OpenAL32.lib;winmm.lib;kernel32.lib;%(AdditionalDependencies);***(...)***\Python\Python37-32\libs\python37.lib 
   in
Linker -> Input -> Additional Dependencies 

in your source code, you'll need 

#include "al.h" 
#include "alc.h" 
#include "alut.h"

In General -> Character Set put Use Multi-Byte Character Set
In Linker -> System -> SubSystem put Console (/SUBSYSTEM:CONSOLE)

Application: Release - Win32
