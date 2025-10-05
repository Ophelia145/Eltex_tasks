FROM ubuntu:latest

RUN apt-get update && apt-get install -y iproute2 make gcc tcpdump

COPY Makefile .
COPY server.c .
COPY client.c .

RUN make
