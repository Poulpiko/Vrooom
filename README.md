# Vrooom

To use OpenAL with Visual Studio, do the following (this 
assumes you have OpenAL installed in the default location  
on drive C:). 

In project properties, 

-- put
C:\Program Files %28x86%29\OpenAL 1.1 SDK\include;%(AdditionalIncludeDirectories);C:\Users\******\AppData\Local\Programs\Python\Python37-32\include
   in
C++ -> General -> Additional Include Directories

-- put 
C:\Program Files %28x86%29\OpenAL 1.1 SDK\libs\Win32;%(AdditionalLibraryDirectories)
   in
Linker -> General -> Additional Library Directories

-- put
OpenAL32.lib;winmm.lib;kernel32.lib;%(AdditionalDependencies);C:\Users\poulp\AppData\Local\Programs\Python\Python37-32\libs\python37.lib 
   in
Linker -> Input -> Additional Dependencies 

in your source code, you'll need 


#include "al.h" 
#include "alc.h" 
#include "alut.h"
