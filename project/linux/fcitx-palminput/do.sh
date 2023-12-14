cd ../../../engine
rm -rf build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
make
mkdir ../lib/linux
cp libpalmengine.a ../lib/linux/
cd ../../project/linux/fcitx-palminput
rm -rf build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
make
#sudo cp build/src/fcitx-googlepinyin.so /usr/lib/x86_64-linux-gnu/fcitx/fcitx-googlepinyin.so

#sudo checkinstall --install=no



