#!/bin/bash -ex

docker build -t renode_bluepill -f renode/Dockerfile .

docker run -v `pwd`:/stm32_bare_lib -it renode_bluepill /stm32_bare_lib/renode/test_hello_world_internal.sh
