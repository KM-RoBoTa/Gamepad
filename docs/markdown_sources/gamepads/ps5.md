\page ps5 PS5

# Original mapping in Linux kernel

The following picture maps the names and values of a PS5 **as defined in the Linux kernel**, aka **before** the remapping applied by this library. \n
Do not use this picture as a reference for the values provided by this library!


![Original PS5 mapping in the Linux kernel](ps5/ps5_evdev.png)

The buttons (in orange) follow a binary on/off logic (0/1). \n
The axes (in green) are described by a value between 0 and 255, following the reference systems as indicated. The HAT0X and HAT0Y are exceptions: they can only have a value of -1, 0 or 1. \n
The 2 blue rectangles on top represent the L2 and R2 buttons near the bottom of the gamepad, which could not be drawn on the picture directly because of the perspective. They act as both a button and an axis.

To read more about it:
- event types: https://www.kernel.org/doc/Documentation/input/event-codes.txt
- event codes: https://gitlab.freedesktop.org/libevdev/libevdev/-/blob/master/include/linux/linux/input-event-codes.h 


# Remapping by this library

As already mentioned, this library remaps some values and some reference systems so that it's more natural to use:

![Remapping of a PS5 by this library](ps5/ps5_KMR_lib.png)

The buttons remain as they were, with a binary logic.\n
The axes get remapped to [-1, 1], and the Y-axes got rotated so that they face towards the front of the gamepad.\n
For the special case of HAT0X and HAT0Y, the y-axis was rotated, but they are treated like buttons by the library.

## Access a PS5's status
The status of the PS5 is stored inside the attributes of the KMR::gamepads::PS5 class object:

- the buttons are stored in KMR::gamepads::PS5::m_buttons[index], where `index` is an enumerate of all buttons found in a PS5, KMR::gamepads::ps5Buttons. So, for example to access the state of the button BTN_SELECT in the object ps5, one must access `ps5.m_buttons[e_BTN_SELECT]`
- the values of axes are accessed in the exact same way, through `ps5.m_axes[index]`, where `index` is an enumerate of KMR::gamepads::ps5Axes.