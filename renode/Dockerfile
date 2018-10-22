# This docker configuration file lets you emulate a Blue Pill board
# on an x86 desktop or laptop, which can be useful for debugging and
# automated testing.
FROM antmicro/renode:latest

LABEL maintainer="Pete Warden <petewarden@google.com>"
WORKDIR /

# Install building tools
RUN apt install -y gcc-arm-none-eabi unzip
RUN curl -L https://github.com/ARM-software/CMSIS_5/archive/5.4.0.zip -o /CMSIS_5.zip
RUN unzip /CMSIS_5.zip
RUN ln -s /CMSIS_5-5.4.0 /CMSIS_5
