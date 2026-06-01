cd /Users/vuillemr/PremierPlugin/build && cmake .. -DCMAKE_BUILD_TYPE=Release 2>&1 && cmake --build . --config Release -j$(sysctl -n hw.ncpu) 2>&1 && cd ..


conda run -n audio-test python test_plugin.py


cd test && cmake -S . -B build && cmake --build build && cd ..
./test/build/test
