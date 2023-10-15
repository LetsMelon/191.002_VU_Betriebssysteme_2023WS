FROM ubuntu:latest

RUN apt update \
    && apt upgrade -y \
    && apt install -y software-properties-common \
    && add-apt-repository ppa:maveonair/helix-editor \
    && apt update \
    && apt install -y gdb gcc make curl helix \
    && curl https://sh.rustup.rs -sSf | sh -s -- -y

ENV PATH=$PATH:/root/.cargo/bin/
RUN cargo install ugdb

WORKDIR /mygrep

COPY . .
