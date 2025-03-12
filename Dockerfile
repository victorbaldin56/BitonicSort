FROM ubuntu:22.04

RUN apt-get update && \
    apt-get install -y pocl-opencl-icd ocl-icd-opencl-dev gcc clinfo python3 pip && \
    python3 -m venv .venv && \
    source .venv/bin/activate && \
    pip install conan && \
    conan profile detect --force
COPY . /app
RUN conan install . --output-folder=build --build=missing && cd build && cmake .. --preset conan-release && cmake --build . -j

WORKDIR /app
