# Raytracer
![CMake](https://github.com/sgooding/Raytracer/workflows/CMake/badge.svg)


[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=sgooding_Raytracer&metric=ncloc)](https://sonarcloud.io/dashboard?id=sgooding_Raytracer)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=sgooding_Raytracer&metric=sqale_rating)](https://sonarcloud.io/dashboard?id=sgooding_Raytracer)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=sgooding_Raytracer&metric=reliability_rating)](https://sonarcloud.io/dashboard?id=sgooding_Raytracer)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=sgooding_Raytracer&metric=security_rating)](https://sonarcloud.io/dashboard?id=sgooding_Raytracer)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=sgooding_Raytracer&metric=bugs)](https://sonarcloud.io/dashboard?id=sgooding_Raytracer)


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

## Docker
```
docker run -it -v /local/dir:/container/dir:rw ghcr.io/sgooding/raytracer:latest bash
cd /container/dir
raytracer /usr/local/examples/5000_corrected_glass.txt
```
