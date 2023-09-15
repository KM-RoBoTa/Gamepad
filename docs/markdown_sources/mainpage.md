# Introduction {#mainpage}
[TOC]

This library provides an easy way to fetch the status of different gamepads (currently only PS5). \n
The events are handled by evdev, the input event interface of the Linux kernel; as such, the libevdev library is used.

## Working concept
The creation of a gamepad class object (ex: PS5) automatically starts a thread that continuously reads and stores the gamepad status into its corresponding attribute arrays. \n
It is up to the user, from the main thread, to fetch values of interest stored in the object's attributes, of course while taking care to lock the object's mutex during said fetching. 

Additionally, for ease of use, this library remaps the values and sometimes the orientation of buttons and axes. Typically, the values gotten back by libevdev are in the interval of [0, 255]; this library remaps them to [-1, 1].
Check the class documentation for more information. 

This library is written in C++.


## Links

- Repository: https://github.com/KM-RoBoTa/KMR_gamepads
- How to [setup](#setup)
- How to [use](#how-to-use)

## Use rights, credentials...?


## Not yet implemented
Currently only works by cable, not by bluetooth yet.
