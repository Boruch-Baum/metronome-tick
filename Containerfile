FROM alpine:edge AS tick-deps
RUN ["apk", "add", "alsa-lib-dev", "make", "musl-dev"]
RUN ["apk", "add", "-X", "http://dl-cdn.alpinelinux.org/alpine/edge/testing", "tcc", "tcc-libs-static"]

FROM tick-deps
WORKDIR /app
COPY lib lib
COPY test test
COPY main.c Makefile .
CMD ["make", "CC=tcc", "test"]
