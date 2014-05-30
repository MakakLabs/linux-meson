linux-meson
===========

Linux kernel sources for Amlogic Meson.

This repository is my attempt to group all the stable kernel releases for the Meson platforms (M1/3/6/8) and their fixes to one repository.

I'm not a kernel developer nor I wish to be, just playing around with Linux on these devices.


Development process
===================

ATM there's no much development happening here, mostly updating the configs and cherry-picking patches from older Amlogic kernel releases (that originally pushed 
by OSS developers) to the latest one.  
As one can notice from the fact this is a fork of torvalds/linux my goal is to have clean patches against Linux mainline, this is a big task for me as a novice 
Linux user (I'm not a developer) so it might take some time to get there (if at all).


Anyone that wants to contribue is welcome to fork this repo but be alert of the following.


I'm still figuring out how to properly manage this repository, some branches might be removed in favor of new repositories with Amlogic drivers, but the overall I 
think the structure will stay the same:
* Stable branches: list bellow.
* Mirror branches: just for mirroring and refernce.
* Upstream: checkout directly from Linux mainline git.
* WIP branches. 
* Feature brnaches: a new feature that in't ready to be pushed to the WIP or stable branch or kept in a separated branch as the local base branch isn't upstream 
(upstream is somewhere else like Corey's repo), so after a PR to upstream the local base will be update from upstream (not be confuse with the branches from Linux 
mainline which named here upstream).


If you're forking this repo then keep in mind that:
* Stable branches are the only ones that their timeline won't be disturbed.
* Feature branches might be squashed and deleted.
* WIP branches might be deleted.
* Staging branches are rebased from stable and updated with cherry-picked commits from feature branches.

Please open an issue if:
* You like to help with WIP branch so I'll know not to disturbed its timeline.
* You find an updated kernel tree so I'll know and add it.


Different approaches to kernel releases
=======================================

Amlogic kernel releases includes predefined cross compile settings and few unneeded binary tools like mkimage.

My approach is that for older releases than 3.10 these settings should be kept as we mainly using these releases with the Buildroot tree for embedded XBMC Linux 
solution and the Buildroot tree already set for these releases.  
For 3.10 released and up these setting and binary blobs should be removed as we can run full armhf distro with these kernels and we should put our efforts in a 
more popular embedded XBMC Linux soultion (hint: it's not Buildroot based).


Roadmap
=======

* Bring wip/2.6.34.15 to a stable state, that mean update with the changes to the board files and configs and cherry-picking commits from 2.6.34 branch.
* Have the most updated and stable kernel for my M3 devices (GBox, Mygica A11, STV01, TLBB, MK802), M6 (G18, Mygica ATV520E, Mygica ATV120).
* Running Arch Linux Arm as a development enviroment.
* Bring my own uboot.
* No timetable, this is done at my free time, I've got other hobbies and exams to learn to.


Stable branches
===============

The latest stable branch for M1/M3 devices is [m3-2.6.34](https://github.com/MakakLabs/linux-meson/tree/m3-2.6.34) which is based on [J1nx-Hackable-Gadgets/buildroot-linux-kernel-m3 develop branch](https://github.com/J1nx-Hackable-Gadgets/buildroot-linux-kernel-m3/tree/develop)

The latest stable branch for M6 devices is [m6-3.0.101](https://github.com/MakakLabs/linux-meson/tree/m6-3.0.101) which is based on [CoreTech-Development/mx-common](https://github.com/CoreTech-Development/mx-common)


Branches in detail
==================
* m3-2.6.34 : supports M1,M3, origin from Pivos repository, commits cherry-picked from on [J1nx-Hackable-Gadgets/buildroot-linux-kernel-m3 develop branch](https://github.com/J1nx-Hackable-Gadgets/buildroot-linux-kernel-m3/tree/develop), reached EOF, it looks like Amlogic privately released 2.6.34-15 (20130701).
* m6-3.0.101 : supports M6, origin from Matricom repository, main development at [CoreTech-Development/mx-common](https://github.com/CoreTech-Development/mx-common), originally it's 3.0.50 release from Amlogic, reached EOF, Amlogic released updated 3.0.50 release (3.0.50-20140314) and 3.10.10 (3.10-20140306).
* wip/2.6.34.15 : supports M1,M3, mali api19, origin from [Stane1983 repository](https://github.com/Stane1983/amlogic-m3).
* wip/3.10.33 : supports M6, M8, origin from [oman007/s82_kernel](https://github.com/oman007/s82_kernel).
