FROM ubuntu:18.04

# Setup image
RUN apt-get update && \
    apt-get install -y \
        git \
        build-essential \
        cmake

# Clone
RUN git clone https://github.com/lawben/sre-smtp.git
WORKDIR /sre-smtp

# Build
RUN mkdir -p build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j smtp-server

# Run
EXPOSE 5555
CMD ./build/smtp-server 5555