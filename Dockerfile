FROM debian:bookworm-slim

RUN apt update && \
    apt install make gcc binutils g++ gcc-multilib g++-multilib -y

WORKDIR /nm

ENTRYPOINT ["/bin/bash"]