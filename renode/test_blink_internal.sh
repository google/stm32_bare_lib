#!/bin/bash -e

cd /stm32_bare_lib
make clean
make

renode -P 5000 --disable-xwt -e 's @/stm32_bare_lib/renode/test_blink.resc'

if grep -q 'gpioPortC.*0x2000\.' /tmp/renode_bluepill_log.txt
then
  echo "LED enabled!"
else
  echo "No LED enable command detected, test failed"
  exit 1
fi

if grep -q 'gpioPortC.*0x20000000\.' /tmp/renode_bluepill_log.txt
then
  echo "LED disabled!"
else
  echo "No LED disable command detected, test failed"
  exit 1
fi

echo "Blink test passed"
