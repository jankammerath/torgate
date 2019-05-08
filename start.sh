#!/bin/sh
docker container rm torgate
docker run --cap-add=SYS_PTRACE -v /Users/jan/Development/torgate:/torgate -p 80:80 -p 9050:9050 --name torgate -it torgate