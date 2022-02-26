FROM ubuntu:latest as base

RUN apt-get update; \
    DEBIAN_FRONTEND=noninteractive \
    TZ="America/New_York" \
    apt-get -y install cmake \
                       git \
                       build-essential


FROM base as builder

COPY . /usr/src/raytracer
RUN cd /usr/src/raytracer; \
       rm -rf build; \
       mkdir build; \
       cd build; \
       cmake ..; make; make install;


FROM ubuntu:latest as deploy

COPY --from=builder /usr/local/bin/raytracer /usr/local/bin
COPY --from=builder /usr/local/examples /usr/local/examples
