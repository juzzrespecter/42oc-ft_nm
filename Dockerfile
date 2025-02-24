FROM debian:bookworm-slim

RUN apt-get update && \
    apt-get install make gcc binutils g++ gcc-multilib g++-multilib -y

WORKDIR /nm

CMD ["/bin/bash"]
