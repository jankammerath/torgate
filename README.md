# torgate

Torgate is a reverse HTTP proxy to access Onion-websites through the Tor network. It is written in C++ for Linux using libmicrohttpd, libcurl and libconfig++. To use torgate you need a Linux operating system and a DNS server or DNS entry with a wildcard to your domain that is then appended to the .onion address. Torgate is very similar to Tor2Web, but written in C++ supporting multi-threading. It also supports Cookies, HTTP POST and header forwarding.

## How to install

Torgate is in the early stages and installation requires compiling the software yourself using the Makefile. However to make things easier, the repository contains a Dockerfile and a docker-compose.yml file which allow you to quickly run Torgate through Docker. If you however are unfamiliar with Docker, it is recommended to install torgate on your Linux box.

### Run on Docker

If you wish to run torgate through Docker, then download the repository and its files either as a ZIP-file or through git. Then just execute docker-compose as below and a container with torgate including the tor client with its socks proxy will be started. The container is configured to run out of the box.

```
$ git clone https://github.com/jankammerath/torgate.git
$ cd torgate/
$ docker-compose up
```

### Install on Ubuntu Bionic 18.04

...

## Configuration

You need to configure your DNS in order for torgate to work. By default the TLD or domain for torgate to append to .onion-Domains is torgate.test. The following line is required to be added to your DNS server. The example shows the configuration for the dnsmasq DNS server.

```                    
address=/torgate.test/192.168.1.1
```

### torgate.conf

The torgate.conf files holds further configuration options.