# Building on Windows with Visual Studio 2013

Open the VS2013 x64 Native Tools Command Prompt. 2012 will *not* work.
Change to the gd-libgd folder.
git clone https://github.com/imazen/gd-win-dependencies into the folder. 
Run 
nmake /f windows/Makefile.vc all
nmake /f windows/Makefile.vc check

Before rebuilding, run:
nmake /f windows/Makefile.vc clean 