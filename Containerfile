FROM alpine:edge
WORKDIR /app
COPY lib lib
COPY test test
COPY tick.c Makefile .
RUN ["apk", "add", "alsa-lib-dev", "clang", "compiler-rt", "make"]
CMD ["make", "test"]
