FROM alpine:edge AS tick-test-deps
RUN ["apk", "add", "alsa-lib-dev", "clang", "compiler-rt", "make"]

FROM tick-test-deps
WORKDIR /app
COPY lib lib
COPY test test
COPY main.c Makefile .
CMD ["make", "test"]
