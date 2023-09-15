\page how-to-use How to use
[TOC]

The library lives in the KMR::gamepads namespace. 

As previously mentioned, when a class object (for example PS5) is created, a thread continuously reading and saving its values is automatically started. \n
To safely stop the thread at the end of the program, the "stop" method needs to be called by the user. 

The library is thus very straightforward to use, without any kind of additional configurations: only the constructor and the "stop" method need to be called. \n
The user needs to implement, in the main thread, a reading function that fetches the gamepad values they're interested in, and must take care to lock the object's attribute mutex during that reading. 


# Example  {#example-code}
This is a very simple example where the user-defined getGamepadData function fetches the gamepad status every second:
```cpp
#include <iostream>
#include <string>
#include <unistd.h>  // Provides sleep function for linux
#include <thread>

#include "KMR_ps5.hpp"


using namespace std;

int main()
{
    KMR::gamepads::PS5 ps5;

    // Start of the main loop
    do{
        getGamepadData(ps5);
        sleep(1);
    } while(1);


    ps5.stop();
}
```

The function getGamepadData reads and prints values read from both thumb axes and the arrows:
```cpp
void getGamepadData(KMR::gamepads::PS5& ps)
{
    // Lock the mutex
    scoped_lock lock(ps.m_mutex);

    // Print the values of both thumb axes and the arrows
    cout << "ABS RX: " << ps.m_axes[KMR::gamepads::e_ABS_RX] << endl;
    cout << "ABS RY: " << ps.m_axes[KMR::gamepads::e_ABS_RY] << endl;
    cout << "ABS X: " << ps.m_axes[KMR::gamepads::e_ABS_X] << endl;
    cout << "ABS Y: " << ps.m_axes[KMR::gamepads::e_ABS_Y] << endl;
    cout << "HATOX: " << ps.m_buttons[KMR::gamepads::e_ABS_HAT0X] << endl;
    cout << "HATOY: " << ps.m_buttons[KMR::gamepads::e_ABS_HAT0Y] << endl;
}
```

# Specific gamepads {#specific-gamepads}

For names and values mapping of specific gamepads, check the list below:
- \subpage ps5 
