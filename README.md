# Raytracer
![CMake](https://github.com/sgooding/Raytracer/workflows/CMake/badge.svg)

Just a simple ray tracer program.

## Install
```
mkdir build
cd build
cmake ..
make
make install
```
## Run
```
cd ..
build/app/raytrace input.txt
```

## Results
![Result](output_test.jpg)

## Deploy
```
mkdir build
cd build
cmake ..
make package
sudo dpkg -i Raytracker-*.deb

# uninstall
sudo dpkg -P raytracer
```