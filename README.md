# torgate

Torgate is a reverse HTTP proxy to access Onion-websites through the Tor network. It is written in C++ for Linux using libmicrohttpd, libcurl and libconfig++. To use torgate you need a Linux operating system and a DNS server or DNS entry with a wildcard to your domain that is then appended to the .onion address. Torgate is very similar to Tor2Web, but written in C++ supporting multi-threading. It also supports Cookies, HTTP POST and header forwarding.

![Screenshot of torgate in Docker with Firefox using it](/screenshot.png)

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

There is no installation, yet and the Makefile does not have an install recipe. However you can easily install torgate with the following commands. These also include the libraries required as well as the tor client and socks proxy.

```
$ cd /usr/src/
$ git clone https://github.com/jankammerath/torgate.git
$ cd torgate/
$ cp config/torrc /etc/tor/torrc
$ cp config/torgate.conf /etc/torgate.conf
$ cp -R html /var/torgate/html
$ make
```

Now run torgate by just executing it.

```
$ torgate
```

Please see the configuration section for further necessary configuration, especially of your DNS server. Currently the application is not daemonized, so you would have to configure a service yourself or simply run it in the background.

## Configuration

You need to configure your DNS in order for torgate to work. By default the TLD or domain for torgate to append to .onion-Domains is torgate.test. The following line is required to be added to your DNS server. The example shows the configuration for the dnsmasq DNS server.

```                    
address=/torgate.test/192.168.1.1
```

### torgate.conf

The torgate.conf files holds further configuration options.

1. **port** (default: 80) *the port that the torgate HTTP server runs on*

2. **tld** (default: torgate.test) *the tld that torgate appends to the onion domains*

3. **proxyHost** (default: 127.0.0.1) *the host address of the tor socks proxy*

4. **proxyPort** (default: 9050) *the port of the tor socks proxy*

5. **userAgent** (default: Torgate/0.1) *the user agent torgate identifies towards onion hosts*

6. **verboseClient** (default: false) *if true, libcurl will produce verbose output*

7. **serverBanner** (default: Torgate/0.1) *the server header that torgate responds with*

8. **localPath** (default: /var/torgate/html) *the path where the html files are stored*