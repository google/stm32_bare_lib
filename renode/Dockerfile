# This docker configuration file lets you emulate a Blue Pill board
# on an x86 desktop or laptop, which can be useful for debugging and
# automated testing.
FROM ubuntu:16.04

LABEL maintainer="Pete Warden <petewarden@google.com>"

# Install mono
RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 3FA7E0328081BFF6A14DA29AA6A19B38D3D831EF
RUN apt update
RUN apt install -y apt-transport-https
RUN echo "deb https://download.mono-project.com/repo/ubuntu stable-xenial main" | tee /etc/apt/sources.list.d/mono-official-stable.list
RUN apt update

# Install renode
RUN apt install -y curl telnet
RUN curl -L https://github.com/renode/renode/releases/download/v1.4.2/renode_1.4.2_amd64.deb -o /tmp/renode_1.4.2_amd64.deb
RUN apt install -y /tmp/renode_1.4.2_amd64.deb
