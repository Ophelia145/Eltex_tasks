FROM ubuntu:latest

RUN apt-get update && apt-get install -y make gcc iproute2 tcpdump

WORKDIR /app

# Копируем Makefile и исходники
COPY ../Makefile .
COPY ../server.c .
COPY ../client.c .

RUN make

CMD ["./server.out"]
