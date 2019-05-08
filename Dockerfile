# use latest ubuntu LTS image
FROM ubuntu:latest

# update system and repos
RUN apt update -y

# install required tools for tor proxy
RUN apt install -y apt-transport-https curl tor tor-geoipdb torsocks

# install required tools for development
RUN apt install -y vim g++ make gdb libconfig++-dev libmicrohttpd-dev libcurl4-openssl-dev

# copy the torrc file
COPY config/torrc /etc/tor/torrc