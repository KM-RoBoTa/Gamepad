/**
 * KM-Robota library
 ******************************************************************************
 * @file            KMR_ps5.hpp
 * @brief           Header for the KMR_ps5.cpp file.
 ******************************************************************************
 * @copyright
 * Copyright 2021-2023 Laura Paez Coy and Kamilo Melo                    \n
 * This code is under MIT licence: https://opensource.org/licenses/MIT
 * @authors  Laura.Paez@KM-RoBota.com, 08/2023
 * @authors  Kamilo.Melo@KM-RoBota.com, 08/2023
 * @authors katarina.lichardova@km-robota.com, 08/2023
 ******************************************************************************
 */

#ifndef KMR_PS5_HPP
#define KMR_PS5_HPP

#include <iostream>
#include <mutex>
#include "libevdev-1.0/libevdev/libevdev.h"
#include <thread>
#include <vector>
#include <unistd.h>


namespace KMR::gamepads
{

/**
 * @brief       Enumerate of all buttons present in a PS5
 * @details     The names follow the naming set by the evdev library:
 * https://gitlab.freedesktop.org/libevdev/libevdev/-/blob/master/include/linux/linux/input-event-codes.h 
 * @note        Even though HAT0X and HAT0Y are technically axes, they are classified as buttons
 *              given that they can only take 3 possible values {-1, 0, 1}
 */
enum ps5Buttons {
    SELECT_BTN, START_BTN, PS_BTN, 
    TRIANGLE_BTN, SQUARE_BTN, CROSS_BTN, CIRCLE_BTN,
    R1_BTN, R2_BTN, L1_BTN, L2_BTN,
    R3_BTN, L3_BTN,
    UP_BTN, DOWN_BTN, LEFT_BTN, RIGHT_BTN,
    NBR_BUTTONS, UNDEF_BUTTONS
};

/**
 * @brief   Enumerate of all axes present in a PS5
 */
enum ps5Axes{
    RSTICK_X, RSTICK_Y,
    LSTICK_X, LSTICK_Y,
    L2_AXIS, R2_AXIS,
    NBR_AXES, UNDEF_AXES
};

/**
 * @brief   Class handling the PS5 monitoring and querying
 */
class PS5 {
public:
    PS5();
    PS5(const char* gamepad_portname);
    ~PS5();

    int getButton(ps5Buttons button);
    float getAxis(ps5Axes axis);
    std::vector<int> getBulkButtons(std::vector<ps5Buttons> buttons);
    std::vector<float> getBulkAxes(std::vector<ps5Axes> axes);

private:
    bool m_stopThread;
    std::thread m_ps5_thread;
    std::mutex m_mutex;

    std::vector<int> m_buttons;
    std::vector<float> m_axes;

    void updateGamepad(input_event ev);
    void gamepadLoop(const char* gamepad_portname);
};

}

#endif