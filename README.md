# STM32 Bare Library

System functions and example code for programming the "Blue Pill"
STM32-compatible micro-controller boards. This is not an officially supported
Google product.

 * [Introduction](#introduction)
 * [What You Need](#what-you-need)
 * [Install OpenOCD on your Pi](#install-openocd-on-your-pi)
 * [Wiring up your Blue Pill](#wiring-up-your-blue-pill)
 * [Building the Examples](#building-the-examples)
 * [Testing OpenOCD](#testing-openocd)
 * [Running Examples](#running-examples)
 * [Debug Output](#debug-output)
 * [Debugging with GDB](#debugging-with-gdb)
 * [Using the Library](#using-the-library)
 * [Testing and Debugging using Emulation](#testing-and-debugging-using-emulation)
 * [Further Reading](#further-reading)

## Introduction

You can now buy "Blue Pill" development boards individually for less than $2.
These devices have ARM Cortex M3 CPUs running at 72MHz, 20KB of SRAM, and 64KB
of flash, and consume far less energy than standard ARM processors. They offer
an amazing amount of computing power for a cheap package that can be run on a
battery for a long time. They are perfect for prototyping running
compute-intensive algorithms like machine learning on embedded systems.

Unfortunately, though their price point makes them very accessible, there are
other barriers to getting started with the boards. They require separate
hardware to program, and some basic wiring skills. The system software that you
need to do anything useful with them is also often proprietary and confusing.
This framework tackles all those problems by standardizing a work flow based on
open software and hardware, providing step-by-step guides for the small amount
of wiring needed, and offering self-contained system functions and examples in
plain C.

## What You Need

Before you get started, you'll need to have a few pieces of hardware available.
No soldering or tools are required though, you should be able to assemble
everything you need by hand. My thanks go to
[Andy Selle](http://www.andyselle.com/) who pioneered the approach that I've
documented below.

-   **Blue Pill Board**. You can usually find these by
    [searching on Ebay](https://www.google.com/search?q=site%3Aebay.com+blue+pill+board)
    or Aliexpress. They come with free shipping, but be aware that can take
    several weeks, so I often choose a faster option. I also recommend picking
    up several boards at once, since they're cheap and spares will come in handy
    in case one is damaged.

-   [**Raspberry Pi 3**](https://www.adafruit.com/product/3055). The Blue Pill
    boards have a CPU and RAM, but don't come with any software pre-installed.
    To do anything useful with them, you have to use an external hardware device
    to program code into their flash memory. You can buy commercial tools to do
    this, but I prefer to use a Pi. They have external IO pins that can be
    controlled directly from software, so they can communicate directly to the
    Blue Pill's pin using the SWD protocol to program, control, and even debug
    the chip's execution.

-   [**Female-to-Female Jumper Wires**](https://www.adafruit.com/product/266).
    We need these to connect the Pi's output pins to the Blue Pill's inputs. Any
    electrical connection will do, but this type of wire will slot snugly onto
    the pins at either end.

## Install OpenOCD on your Pi

These steps assume you have a Raspberry Pi 3 running a recent version of
Raspbian. There's nothing that I know definitely won't work on a Pi 2 (or a Pi
Zero/One with an adjustment to the OpenOCD scripts), but I've not tested any
other combinations. The approach I'm using came from
[a great AdaFruit guide](https://learn.adafruit.com/programming-microcontrollers-using-openocd-on-raspberry-pi),
which I recommend checking out for more background on what we're doing.

The main piece of software we need to install is [OpenOCD](http://openocd.org),
the Open On-Chip Debugger. Despite its name, this tool also handles flashing
micro-controller flash memory with new programs, as well as displaying debug
information from the chip. It does this through the Serial Wire Debug protocol,
communicating through dedicated pins on the Blue Pill board.

OpenOCD doesn't have a binary package for the Pi, but it is easy to compile. To
build it, run these commands in the terminal on your Pi:

```
sudo apt-get update
sudo apt-get install git autoconf libtool make pkg-config libusb-1.0-0 libusb-1.0-0-dev telnet
git clone git://git.code.sf.net/p/openocd/code openocd-code
cd openocd-code
./bootstrap
./configure --enable-sysfsgpio --enable-bcm2835gpio
make
sudo make install
```

## Wiring up your Blue Pill

The four pins at the thin end of the Blue Pill board are dedicate to the SWD
protocol, and need to be connected to the right pins on the Pi's header, which
OpenOCD will then control to program the chip.

Here's a diagram showing how the wires should be attached between the Pi and the
board:

![Blue Pill Wiring](https://storage.googleapis.com/download.tensorflow.org/example_images/blue_pill_wiring.png)

This is based on the wiring recommended by the AdaFruit tutorial, where ground
and power (3.3V) can be driven by any of the multiple pins on the Pi that supply
them, SWDIO (the data pin) is on pin 24, and SWDCLK (the clock pin) is on pin
25. I'm leaving the reset pin unconnected, since it's optional.

![AdaFruit Diagram](https://cdn-learn.adafruit.com/assets/assets/000/031/318/large1024/raspberry_pi_SWDPinoutPi2.png)

Here are some photos of my actual setup:

![Blue Pill Wiring](https://storage.googleapis.com/download.tensorflow.org/example_images/blue_pill_0.jpg)

![Pi Wiring](https://storage.googleapis.com/download.tensorflow.org/example_images/blue_pill_1.jpg)

![Complete Picture of Wiring](https://storage.googleapis.com/download.tensorflow.org/example_images/blue_pill_2.jpg)

If you're using female-to-female jumper wires, you should be able to just push
the connectors at each end onto the corresponding pins, and they should fit
snugly with a small amount of force.

There are also two yellow plastic jumper switches on the board. Normally they
will both in position furthest away from the SWD pins, and closest to the small
USB port. This is the recommended position for our workflow, but if they're in a
different place when you get your board, you may need to move them back.

Once you've wired in the ground and power (3.3v) connections, you should see a
red LED on the board light up. If it doesn't, check your wiring, or try another
board in case there's a defect.

## Building the Examples

Now you should be ready to build some example programs for the Blue Pill board.
To do this, you'll need to clone
[ARM's CMSIS 5 micro-controller library](https://github.com/ARM-software/CMSIS_5)
from GitHub and get this repository too. You'll also need to install the
cross-compilation toolchain for ARM.

```
cd ~
git clone https://github.com/ARM-software/CMSIS_5
git clone https://github.com/google/stm32_bare_lib
sudo apt-get install -y gcc-arm-none-eabi
```

Now you should just be able to build the library and examples by running:

```
cd ~/stm32_bare_lib
make
```

If this doesn't produce an error you should have binary files ready to be
flashed to the Blue Pill in `gen/bin/examples/blink.bin`,
`gen/bin/examples/hello_world.bin`, and so on.

These are not quite like normal executables, they're just exact copies of the
bytes that need to be copied into flash memory on the device. They don't contain
any debug symbols, and the very start of flash has to be a table of function
pointers, so making sure that `boot.s` is first in linking order is important.
You don't need to worry about that for these examples, because the makefile
takes care of all that, but once you start building your own programs you'll
need to be careful.

## Testing OpenOCD

Once you've got this library's repository from GitHub, you can start testing
your wiring connection. There are a lot of settings you need to pass to OpenOCD
to make a successful link, so to make things easier these are included in
[opened.cfg](https://github.com/petewarden/stm32_bare_lib/blob/master/openocd.cfg)
in the root of the source tree. To try out OpenOCD, run:

```
cd ~/stm32_bare_lib
sudo openocd -f openocd.cfg
```

If it's working correctly, you should see output like this:

```
Open On-Chip Debugger 0.10.0+dev-00299-g6d390e1b (2018-02-17-00:10)
Licensed under GNU GPL v2
For bug reports, read
    http://openocd.org/doc/doxygen/bugs.html
BCM2835 GPIO config: tck = 11, tms = 25, tdi = 10, tdo = 9
BCM2835 GPIO nums: swclk = 11, swdio = 25
BCM2835 GPIO nums: swclk = 25, swdio = 24
adapter speed: 1000 kHz
adapter_nsrst_delay: 100
none separate
cortex_m reset_config sysresetreq
none separate
Info : BCM2835 GPIO JTAG/SWD bitbang driver
Info : JTAG and SWD modes enabled
Info : clock speed 1001 kHz
Info : SWD DPIDR 0x1ba01477
Info : stm32f1x.cpu: hardware has 6 breakpoints, 4 watchpoints
Info : Listening on port 3333 for gdb connections
semihosting is enabled
Info : Listening on port 6666 for tcl connections
Info : Listening on port 4444 for telnet connections
```

If you see `Error: Could not initialize the debug port` instead, that's a sign
that's something is wrong with the connection. Check your wiring, and if that's
okay make sure that you have the correct pins on your Pi hooked up.

## Running Examples

With OpenOCD running in one terminal, open up a new window and type:

```
telnet localhost 4444
```

This should bring you to a command console for OpenOCD that looks something like
this:

```
Trying ::1...
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Open On-Chip Debugger
>
```

This is the place where you can execute commands to control the Blue Pill board.
The first thing we need is to stop the chip so we can program it. We do that by
running the `reset halt` command, which means restart the CPU and then stop
before executing any instructions.

```
> reset halt
target halted due to debug-request, current mode: Thread
xPSR: 0x01000000 pc: 0x00000052 msp: 0x20005000, semihosting
>
```

Next we will upload one of the binary example programs we've built with `flash
write_image erase gen/bin/examples/blink.bin 0x08000000`. This is telling
OpenOCD to write the data in the file, starting at address `0x08000000`, which
is where flash is mapped to on the Blue Pill.

```
> flash write_image erase gen/bin/examples/blink.bin 0x08000000
auto erase enabled
device id = 0x20036410
flash size = 128kbytes
wrote 1024 bytes from file gen/bin/examples/blink.bin in 0.096076s (10.408 KiB/s)
>
```

If it complains that the file isn't found, make sure that the examples built
correctly in the earlier step, and that the `openocd` command was run from the
stm32_bare_lib source folder.

Finally we need to restart the chip again, at which point it should try to
execute the program we've just uploaded to flash. We do this with the plain
`reset` command, which with no arguments restarts the chip and begins execution.

```
> reset
>
```

If you look at the board, you should see a green LED just below the red power
LED flashing a couple of times a second. Congratulations, you've run your first
micro-controller program!

## Debug Output

Blinking LEDs is fun, but to be at all productive on complex programs we need to
be able to easily pass information back to the host computer for debugging
purposes. The
[Hello World example](https://github.com/petewarden/stm32_bare_lib/blob/master/examples/hello_world/hello_world_main.c)
shows how to use
[`DebugLog()`](https://github.com/petewarden/stm32_bare_lib/blob/master/include/debug_log.h#L24)
to do this. To run it, execute these commands in the OpenOCD command console:

```
reset halt
flash write_image erase gen/bin/examples/hello_world.bin 0x08000000
reset
```

If you switch back to the first terminal that you started the `openocd` command
in (not the console you just used), you should see the line "Hello World!" at
the bottom.

The `DebugLog()` function can take several hundred milliseconds to execute,
since it has to call back to the host machine, so it shouldn't be used in
performance-critical code, but it is handy when you're trying to track down
issues.

## Debugging with GDB

The standard Gnu debugger `gdb` works reasonably well with this setup. To
install it, run:

```
sudo apt-get install -y gdb-arm-none-eabi
```

You can then debug a program by executing:

```
cd ~/stm32_bare_lib/
arm-none-eabi-gdb -iex 'add-auto-load-safe-path .' gen/elf/examples/blink.elf
```

You should see a gdb prompt, and you can execute commands to inspect variables:

For example, recompile and load the binary to the device again

```
(gdb) make
...
(gdb) load
target halted due to debug-request, current mode: Thread
xPSR: 0x01000000 pc: 0x080001c4 msp: 0x20005000, semihosting
Loading section .interrupt_vector, size 0x118 lma 0x8000000
Loading section .text, size 0x190 lma 0x8000118
Loading section .init, size 0xc lma 0x80002a8
Loading section .fini, size 0xc lma 0x80002b4
target halted due to debug-request, current mode: Thread
xPSR: 0x01000000 pc: 0x080001c4 msp: 0x20005000, semihosting
Start address 0x80001c4, load size 704
Transfer rate: 3 KB/sec, 176 bytes/write.
shift = 536891384
old_config = 2101681877
cleared_config = 172
```

Then you can rerun the program. Never use run, always use load and then
continue.

```
(gdb) cont
Continuing.
```

And interrupt it...

```
(PRESS CTRL-C)
Program received signal SIGINT, Interrupt.
0x08000276 in BusyWaitMicroseconds (us=200000) at ./include/timers.h:37
37       for (; count > 0; --count)
```

Inspect variables...

```
(gdb) p count
$1 = 103869
```

Inspect the stack...

```
(gdb) backtrace

# 0 0x08000272 in BusyWaitMicroseconds (us=200000) at ./include/timers.h:37

# 1 0x08000296 in OnReset () at examples/blink/blink_main.c:30

# 2 0x080001d4 in _main () at source/startup.c:66

```

Set breakpoints, inspect stacks, etc. all like you do in normal self-hosted gdb.

## Using the Library

If you want to create your own programs using this framework, you should start
off by copying one of the examples and expanding it. You'll need to:

-   Add new makefile rules for your program.
-   Link against the output of
    [boot.s](https://github.com/petewarden/stm32_bare_lib/blob/master/source/boot.s)
    as the first object in the linking stage. This should happen automatically
    when you use the makefile rules, but it's important since this assembly
    contains the table of function pointers used to call our program code and it
    has to be at the start.
-   Link using `-T stm32_linker_layout.lds`, so the linker knows where program
    and data memory start on the Blue Pill.
-   Instead of a `main()` function, provide `OnReset()`. This is called when the
    chip starts up.
-   Be aware that global variables aren't set up by default, so you need to do
    all initialization explicitly at the start of your `OnReset()` call.

When you need to call more advanced capabilities on the micro-controller, there
are a couple of different approaches. Arm's CMSIS library defines a lot of
device registers in
[`core_cm3.h`](https://github.com/ARM-software/CMSIS/blob/master/CMSIS/Include/core_cm3.h).
It can't be called without some extra definitions beforehand though, so
stm32_bare_lib's
[`core_stm32.h`](https://github.com/petewarden/stm32_bare_lib/blob/master/include/core_stm32.h)
header handles setting those up for the Blue Pill.

Device manufacturers typically also release a header file that defines registers
for peripherals that are not standard across all particular CPUs but that exist
only on their boards. Since Blue Pill SoCs are STM32-compatible, these would
normally be in something like stm32f*.h, but this is only available as part of
proprietary products from ST. Instead, the
[`core_stm32.h`](https://github.com/petewarden/stm32_bare_lib/blob/master/include/core_stm32.h)
header includes a few of the most commonly-used device registers, as defined by
[ST's reference guide to the processor](http://www.st.com/content/ccc/resource/technical/document/reference_manual/59/b9/ba/7f/11/af/43/d5/CD00171190.pdf/files/CD00171190.pdf/jcr:content/translations/en.CD00171190.pdf).
These definitions are used to set up and control the LEDs for example.

## Testing and Debugging using Emulation

The [Renode project](https://renode.io/) is an emulation framework for
microcontrollers, a bit like Docker for embedded systems. It lets you run
software built for systems like the STM32 in an emulated environment on a
standard x86 Linux or MacOS machine. The full scope of the project is too large
to fit in this document, but it's particularly useful when you want to test that
code's running correctly without needing a physical board connected to your
testing machine.

To show how this works, you can try running the
[`renode/test_hello_world.sh`](https://github.com/google/stm32_bare_lib/blob/master/renode/test_hello_world.sh)
script on any machine with Docker and Bash installed. This calls a
[Dockerfile](https://github.com/google/stm32_bare_lib/blob/master/renode/Dockerfile)
that installs Renode and its dependencies into an Ubuntu 16.04 virtual machine,
and then builds and runs
[the hello world example](https://github.com/google/stm32_bare_lib/blob/master/examples/hello_world/)
in that environment. It then examines the log output, and checks that the
expected debug log message has been output, returning success if it has.

This bundling of building, running, and testing the code into a self-contained
script means that it's possible to integrate with standard continuous testing
systems. It also gives a good foundation if you'd like to experiment with renode
emulation for interactive development and debugging. You can build the Docker
image yourself by running:

```bash
docker build -t renode_bluepill -f renode/Dockerfile .
```
This will take a few minutes the first time, since the Renode dependencies are
quite large, but subsequent runs should be much faster thanks to the cache. Then, use:

```bash
docker run -v `pwd`:/stm32_bare_lib -it renode_bluepill /stm32_bare_lib/renode/test_hello_world_internal.sh
```

to execute the test (this is the same command that is used by `test_hello_world.sh`).

What this does is build the [hello world example](https://github.com/google/stm32_bare_lib/blob/master/examples/hello_world/hello_world_main.c),
run it in the emulator, and then check the logs to ensure that the expected
"Hello World!" string was output. Accessing the logs like this makes it
possible to create complex tests that output their results to the host through
`DebugLog()` calls.

You can also open up a shell in the virtual machine with Renode installed by
running:

```bash
docker run -v `pwd`:/stm32_bare_lib -it renode_bluepill
```
You should now find yourself at a root prompt inside the VM. You can run the
hello world test manually with this command:

```bash
renode -P 5000 --disable-xwt -e 's @/stm32_bare_lib/renode/test_hello_world.resc'
```
If you want to use Renode's monitor interactively, you'll need to open up two
terminal windows. In the first one, run docker with the `docker run -v` command
above and then start the renode process by running:

```bash
renode -P 5000 --disable-xwt
```

In the second window, connect to the running docker instance, and then telnet to
the monitor using these commands:

```bash
docker exec -ti $(docker ps -q |head -n1) bash
telnet localhost 5000
```

You should now find yourself with a `[Monitor]` prompt in the second window.
This will allow you to run
[the full set of Renode commands](https://renode.readthedocs.io/en/latest/introduction/demo.html)
to load and run microcontroller executables.

## Further Reading

Looking through the
[examples folder](https://github.com/petewarden/stm32_bare_lib/tree/master/examples)
should give you some ideas on the sort of things that are possible.

[STM32duino](http://wiki.stm32duino.com/index.php?title=Blue_Pill) has a wealth
of information on Blue Pill boards, and the name even comes from
[a thread on their forums](http://www.stm32duino.com/viewtopic.php?f=28&t=117&hilit=blue+pill).

[Thomas Trebisky has some fantastic examples](https://github.com/trebisky/stm32f103)
of bare-metal STM32 programming, and I've found them very useful to reference as
I've been working on this.

[David Welch has also made available great sample code](https://github.com/dwelch67/stm32_samples/tree/master/STM32F103C8T6)
for the STM32.

[Reference Manual](http://www.st.com/content/ccc/resource/technical/document/reference_manual/59/b9/ba/7f/11/af/43/d5/CD00171190.pdf/files/CD00171190.pdf/jcr:content/translations/en.CD00171190.pdf) -
The ultimate resource for understanding what the micro-controller can do.
