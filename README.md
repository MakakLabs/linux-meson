linux-meson
===========

Linux kernel sources for Amlogic Meson.

This repository is my attempt to group all the stable kernel releases for the Meson platforms (M1/3/6/8) and their fixes to one repository.

I'm not a kernel developer nor I wish to be, just playing around with Linux on these devices.


If you're forking this repo then keep in mind that:
* Stable branches are the only ones that their timeline won't be disturbed.
* Feature branches might be squashed and deleted.
* WIP branches might be deleted.
* Staging branches are rebased from stable and updated with cherry-picked commits from feature branches.

Please open an issue if:
* You like to help with WIP branch so I'll know not to disturbed its timeline.
* You find an updated kernel tree so I'll know and add it.



Stable branches
===============

The latest stable branch for M1/M3 devices is [m3-2.6.34](https://github.com/MakakLabs/linux-meson/tree/m3-2.6.34) which is based on [J1nx-Hackable-Gadgets/buildroot-linux-kernel-m3 develop branch](https://github.com/J1nx-Hackable-Gadgets/buildroot-linux-kernel-m3/tree/develop)

The latest stable branch for M6 devices is [m6-3.0.101](https://github.com/MakakLabs/linux-meson/tree/m6-3.0.101) which is based on [CoreTech-Development/mx-common](https://github.com/CoreTech-Development/mx-common)



Branches in detail
==================
* m3-2.6.34 : supports M1,M3, origin from Pivos repository, based on [J1nx-Hackable-Gadgets/buildroot-linux-kernel-m3 develop branch](https://github.com/J1nx-Hackable-Gadgets/buildroot-linux-kernel-m3/tree/develop), reached EOF, it looks like Amlogic privately released 2.6.34-15 (20130701).
* m6-3.0.101 : supports M6, origin from Matricom repository, based on [CoreTech-Development/mx-common](https://github.com/CoreTech-Development/mx-common), originally it's 3.0.50 release from Amlogic, reached EOF, Amlogic released updated 3.0.50 release (3.0.50-20140314) and 3.10.10 (3.10-20140306).
* wip/3.0.50-20140314 : supports M6 (M3 ?), origin from public Amlogic release.
* wip/3.10.10-20140306 : supports M6, M8, origin from public Amlogic release.
