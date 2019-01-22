FROM debian:buster as build
SHELL ["/bin/bash", "-exo", "pipefail", "-c"]

RUN apt-get update ;\
	DEBIAN_FRONTEND=noninteractive apt-get install --no-install-{recommends,suggests} -y \
	cmake make g++ libboost-all-dev ;\
	apt-get clean ;\
	rm -vrf /var/lib/apt/lists/*

ADD . /src

RUN cd /src ;\
	mkdir build ;\
	cd build ;\
	cmake .. ;\
	make


FROM debian:buster
SHELL ["/bin/bash", "-exo", "pipefail", "-c"]

RUN apt-get update ;\
	DEBIAN_FRONTEND=noninteractive apt-get install --no-install-{recommends,suggests} -y \
	libboost-{atomic,chrono,context,coroutine,date-time,system,thread}1.67.0 ;\
	apt-get clean ;\
	rm -vrf /var/lib/apt/lists/*

COPY --from=build /src/build/grumpycathttpd /usr/local/bin

CMD exec grumpycathttpd
