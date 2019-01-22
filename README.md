# GrumpycatHTTPd

A C++ HTTPd powered by Boost ASIO.

## Build

```
mkdir build
pushd build
cmake ..
make
```

## Run

```
./grumpycathttpd
```

## Test

```
wrk -d 30 -c 100 -t 100 http://127.0.0.1:8910/
```
