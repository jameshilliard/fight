call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat"
cd liveMedia
nmake /B -f liveMedia.mak clean
cd ../groupsock
nmake /B -f groupsock.mak clean
cd ../UsageEnvironment
nmake /B -f UsageEnvironment.mak clean
cd ../BasicUsageEnvironment
nmake /B -f BasicUsageEnvironment.mak clean
cd ../testProgs
nmake /B -f testProgs.mak clean
cd ../mediaServer
nmake /B -f mediaServer.mak clean
pause