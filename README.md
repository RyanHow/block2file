block2file
==========

Fuse Filesystem to mount a block device as a single image file

This allows tools like rsync to work with a block device

This currently only does the very bare minimum to function. Only mounts read only. Just sharing it here incase anyone else wants this, or wants to help develop. I couldn't find anything else on the web that allowed rsyncing a block device (apart from rsync patches which seemed somewhat flaky).

Disclaimer: I'm not a C programmer, so my code might not be the greatest :), but it's been working in production.

Usage
=====
block2file blockdevice mountpoint [options]

Example
=======

> block2file /dev/sda /mnt/sda_block

> ls mnt/sda_block

device.img

> du -h

4.0G

Compiling
=========
gcc -Wall block2file.c `pkg-config fuse --cflags --libs` -o block2file
