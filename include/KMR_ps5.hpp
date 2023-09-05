/**
 * KM-Robota library
 ******************************************************************************
 * @file            KMR_ps5.hpp
 * @brief           Header for the KMR_ps5.cpp file.
 ******************************************************************************
 * @copyright
 * Copyright 2021-2023 Laura Paez Coy and Kamilo Melo                    \n
 * This code is under MIT licence: https://opensource.org/licenses/MIT
 * @authors  Laura.Paez@KM-RoBota.com, 04-2023
 * @authors  Kamilo.Melo@KM-RoBota.com, 05-2023
 ******************************************************************************
 */

#ifndef KMR_PS5_HPP
#define KMR_PS5_HPP

#include <iostream>
#include <mutex>
#include "libevdev-1.0/libevdev/libevdev.h"
#include <thread>


namespace KMR::gamepads
{

/**
 * @brief       Enumerate of all buttons present in a PS5
 * @details     The names follow the naming set by the evdev library:
 * https://gitlab.freedesktop.org/libevdev/libevdev/-/blob/master/include/linux/linux/input-event-codes.h 
 * @note        Even though HAT0X and HAT0Y are technically axes, they are classified as buttons
 *              given that they can only take 3 possible values {-1, 0, 1}
 */
enum ps5Buttons {e_BTN_SELECT, e_BTN_START, e_BTN_MODE, 
                 e_BTN_NORTH, e_BTN_WEST, e_BTN_SOUTH, e_BTN_EAST,
                 e_BTN_TR, e_BTN_TR2, e_BTN_TL, e_BTN_TL2,
                 e_BTN_THUMBR, e_BTN_THUMBL,
                 e_ABS_HAT0X, e_ABS_HAT0Y, 
                 NBR_BUTTONS, UNDEF_BUTTON};

/**
 * @brief       Enumerate of all axes present in a PS5
 */
enum ps5Axes{e_ABS_RX, e_ABS_RY, e_ABS_X, e_ABS_Y,
             e_ABS_Z, e_ABS_RZ,
             NBR_AXES, UNDEF_AXIS};

class PS5 {
    private:
        int m_fd; // File descriptor
        std::thread m_ps5_thread;

        void updateGamepad(input_event ev);
        void gamepadLoop(const char* gamepad_portname);

    public:
        int m_buttons[NBR_BUTTONS];
        float m_axes[NBR_AXES];
        std::mutex m_mutex;

        PS5();
        PS5(const char* gamepad_portname);
        void stop();
};

}

#endif