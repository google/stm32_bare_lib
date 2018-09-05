#!/bin/bash -e

cd /stm32_bare_lib
make clean
make

renode -P 5000 --disable-xwt -e 's @/stm32_bare_lib/renode/test_hello_world.resc'

if grep -q 'Hello World!' /tmp/renode_bluepill_log.txt
then
  echo "'Hello World!' string detected"
else
  echo "No 'Hello World!' string detected, test failed"
  exit 1
fi

echo "Hello world test passed"
