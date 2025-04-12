#!/bin/sh

if [ "$1" == "build" ]; then
	docker build --platform linux/amd64 -t arch-bash .
fi

docker run -it --rm --platform linux/amd64 -v $(pwd):/workspace arch-bash
