FROM debian:bullseye-slim

WORKDIR /mirrorserver

RUN apt update && apt upgrade -y \
    && apt install -y cmake \
    g++ make wget bash

COPY . .

RUN ./prepare.sh && cd server && \
    cmake . && make

WORKDIR /mirrorserver/server

CMD ["./ntechlab_testtask", "-i"] 