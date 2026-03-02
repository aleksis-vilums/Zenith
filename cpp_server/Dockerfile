FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libprotobuf-dev \
    protobuf-compiler \
    libgrpc++-dev \
    libgrpc-dev \
    protobuf-compiler-grpc \
    libabsl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY cpp_server/ .

COPY protos/ ../protos

RUN mkdir build
WORKDIR /app/build

RUN cmake ..
RUN cmake --build . -j 4

EXPOSE 50051

CMD ["./zenith_server"]