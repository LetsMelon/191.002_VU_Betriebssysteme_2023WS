FROM ubuntu:latest

RUN apt update \
    && apt upgrade -y \
    && apt install -y software-properties-common \
    && add-apt-repository ppa:maveonair/helix-editor \
    && apt update \
    && apt install -y gdb gcc make curl helix valgrind nano man \
    && curl https://sh.rustup.rs -sSf | sh -s -- -y

ENV PATH=$PATH:/root/.cargo/bin/
RUN cargo install ugdb

WORKDIR /3coloring

COPY . .

# FROM centos:centos7.9.2009
# 
# RUN yum install -y make gcc curl gdb \
#     && curl https://sh.rustup.rs -sSf | sh -s -- -y
# 
# ENV PATH=$PATH:/root/.cargo/bin/
# 
# WORKDIR /3coloring
# 
# COPY . .
