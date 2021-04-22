::DO_NOT_CHANGE::
::============================================::
set "cvVersion=3.4.14"
echo Installing OpenCV-%cvVersion%
::============================================::
mkdir opencv-%cvVersion%
cd opencv-%cvVersion%
mkdir Installation
set count=1
set "cwd=%cd%"
::============================================::
echo "Downloading opencv from github"
::download opencv from git
git clone https://github.com/opencv/opencv.git
cd opencv
::checkout appropriate cv version
git checkout %cvVersion%
cd ..
::============================================::
echo "Compiling using cmake"
cd opencv
mkdir build
cd build
::xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx::
::DO_NOT_CHANGE::
cmake -G "Visual Studio 16 2019" -T host=x64 -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=%cwd%/Installation -D BUILD_opencv_java=OFF -D BUILD_opencv_python=OFF ..
::DO_NOT_CHANGE::
::============================================::
::Compile OpenCV in release mode
cmake.exe --build . --config Release --target INSTALL
cd ..
cd ..
cd ..