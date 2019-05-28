# Vrooom

To use OpenAL with Visual Studio, do the following (this 
assumes you have OpenAL installed in the default location  
on drive C:). 

In project properties, 

-- add 
C:\Program Files\OpenAL 1.1 SDK\include to 
C++ -> General -> Additional Include Directories

-- add  C:\Program Files\OpenAL 1.1 SDK\libs\Win32
   to Linker -> General -> Additional Library Directories

-- add  ;OpenAL32.li;ALut.lib at the end  to 
   Linker -> Input -> Additional Dependencies 

in your source code, you'll need 


#include "al.h" 
#include "alc.h" 
#include "alut.h"
