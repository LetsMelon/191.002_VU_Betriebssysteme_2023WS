FROM ubuntu:latest AS RUST_DEP

RUN apt update \
    && apt install -y curl git gcc \
    && curl https://sh.rustup.rs -sSf | sh -s -- -y

ENV PATH=$PATH:/root/.cargo/bin/

RUN git clone https://github.com/ftilde/ugdb.git \
    && cd ugdb \
    && git fetch --tags \
    && git checkout tags/0.1.11 \
    && cargo build --release

FROM ubuntu:latest

RUN echo 'alias nc="ncat"' > /root/my_aliases.sh
RUN echo 'source /root/my_aliases.sh' >> /root/.bashrc

RUN apt update \
    && apt upgrade -y \
    && apt install -y software-properties-common \
    && add-apt-repository ppa:maveonair/helix-editor \
    && apt update \
    && apt install -y gdb gcc make helix valgrind nano man man-db ncat

COPY --from=RUST_DEP /ugdb/target/release/ugdb /bin

WORKDIR /http

COPY . .
