FROM ubuntu:24.04
RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ cmake make ca-certificates \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY CMakeLists.txt ./
COPY include include
COPY src src
COPY tests tests
COPY benchmarks benchmarks
RUN cmake -S . -B build -DCOREX_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build -j$(nproc)
CMD ["ctest", "--test-dir", "build", "--output-on-failure"]
