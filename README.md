# torgate

Torgate is a reverse HTTP proxy to access Onion-websites through the Tor network. It is written in C++ for Linux using libmicrohttpd, libcurl and libconfig++. To use torgate you need a Linux operating system and a DNS server or DNS entry with a wildcard to your domain that is then appended to the .onion address. Torgate is very similar to Tor2Web, but written in C++ supporting multi-threading. It also supports Cookies, HTTP POST and header forwarding.

## How to install

Torgate is in the early stages and installation requires compiling the software yourself using the Makefile.