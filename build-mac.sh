
# Configure and Build PlotJuggler
cmake -S src/PlotJuggler -B build/PlotJuggler -DCMAKE_INSTALL_PREFIX=install #-DBASE_AS_SHARED=ON
cmake --build build/PlotJuggler --config RelWithDebInfo --parallel --target install

# Configure and Build Balyrond Plugins
cmake -S src/balyrond-pj-plugins -B build/balyrond-pj-plugins \
      -DCMAKE_INSTALL_PREFIX="`pwd`/install" \
      -DCMAKE_PREFIX_PATH="`pwd`/install"

cmake --build  build/balyrond-pj-plugins --config RelWithDebInfo --target install --parallel
