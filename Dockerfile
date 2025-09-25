FROM alpine:3.18

RUN apk add --no-cache \
    g++ \
    cmake \
    make \
    git \
    libstdc++ \
    libgcc

RUN git clone --depth 1 https://github.com/google/googletest.git /usr/src/googletest && \
    mkdir -p /usr/src/googletest/build && \
    cd /usr/src/googletest/build && \
    cmake .. && make && make install

WORKDIR /app

COPY ./src/ ./src/
COPY ./include/ ./include/
COPY ./tests/ ./tests/
COPY ./Makefile ./Makefile
COPY ./trading_hours.ini ./trading_hours.ini

RUN make test

ENTRYPOINT ["make"]
CMD ["run"]