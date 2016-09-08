mkdir -p build
cd build
cmake -DPREFIX=build -DPYTHON_LIBRARY=/usr/local/Cellar/python/2.7.12/Frameworks/Python.framework/Versions/2.7/lib/libpython2.7.dylib ..
make
