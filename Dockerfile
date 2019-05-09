# use latest ubuntu LTS image
FROM ubuntu:latest

# update system and repos
RUN apt update -y

# install required tools for tor proxy
RUN apt install -y apt-transport-https curl tor tor-geoipdb torsocks

# install required tools for development
RUN apt install -y vim g++ make gdb libconfig++-dev libmicrohttpd-dev libcurl4-openssl-dev libmagic-dev

# copy the torrc and torgate.conf file
COPY config/torrc /etc/tor/torrc
COPY config/torgate.conf /etc/torgate.conf

# copy the html files
COPY html /var/torgate/html/

# copy the source files and compile
COPY src /usr/src/torgate/src/
COPY Makefile /usr/src/torgate/Makefile
WORKDIR /usr/src/torgate
RUN make
CMD service tor start && torgate