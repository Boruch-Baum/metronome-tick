FROM alpine:edge AS tick-deps
RUN ["apk", "add", "alsa-lib-dev", "gcc", "make", "musl-dev"]

FROM tick-deps
WORKDIR /app
COPY lib lib
COPY test test
COPY main.c Makefile .
CMD ["make", "test"]
