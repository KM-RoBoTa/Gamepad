/**
 ****************************************************************************
 * KM-Robota's ps5.cpp module
 ****************************************************************************
 * @file        KMR_ps5.cpp
 * @brief       ps5
 ****************************************************************************
 * @copyright
 * Copyright 2021-2023 Laura Paez Coy and Kamilo Melo                    \n
 * This code is under MIT licence: https://opensource.org/licenses/MIT
 * @authors  Laura.Paez@KM-RoBota.com, 08/2023
 * @authors  Kamilo.Melo@KM-RoBota.com, 08/2023
 * @authors katarina.lichardova@km-robota.com, 08/2023
 ****************************************************************************
 */

// Event types, directly defined in the linux kernel: 
// https://www.kernel.org/doc/Documentation/input/event-codes.txt
// Event codes, defined in libevdev: 
// https://gitlab.freedesktop.org/libevdev/libevdev/-/blob/master/include/linux/linux/input-event-codes.h

#include <iostream>
#include <fcntl.h>   // Control over the open file referenced by file descriptor 
#include <string.h> 

#include "KMR_ps5.hpp"


using namespace std;

#define MAX_AXIS_VAL 255

namespace KMR::gamepads
{

/**
 * @brief       Create and initialize a PS5 object
 * @retval      void
 * @note        By default, open Sony Dualsense gamepad. If you wish to use a different
 *              gamepad, use the overload of this constructor
 */
PS5::PS5()
{
    m_stopThread = false;

    // Initialize tables
    for (int i=0; i<NBR_BUTTONS; i++)
        m_buttons[i] = 0;
    for (int i=0; i<NBR_AXES; i++)
        m_axes[i] = 0; 

    m_ps5_thread = thread(&KMR::gamepads::PS5::gamepadLoop, this,
    "/dev/input/by-id/usb-Sony_Interactive_Entertainment_Wireless_Controller-if03-event-joystick");

    cout << "Success: PS5 object created!" << endl;
}


/**
 * @brief       Create and initialize a PS5 object with a different gamepad than the default
 * @retval      void
 * @overload
 */
PS5::PS5(const char* gamepad_portname)
{
    // Initialize tables
    for (int i=0; i<NBR_BUTTONS; i++)
        m_buttons[i] = 0;
    for (int i=0; i<NBR_AXES; i++)
        m_axes[i] = 0; 

    m_ps5_thread = thread(&KMR::gamepads::PS5::gamepadLoop, this, gamepad_portname);

    cout << "Success: PS5 object created!" << endl;
}


/**
 * @brief       Class destructor. Takes care of safely stopping the thread
 */
PS5::~PS5()
{
    m_stopThread = true;

    if (m_ps5_thread.joinable()) 
        m_ps5_thread.join();
}


/**
 * @brief       Read and save the current gamepad values. To loop in a thread
 * @param[in]   gamepad_portname Port handling the gamepad, in the form of /dev/input/eventX
 * @retval      void
 */
void PS5::gamepadLoop(const char* gamepad_portname)
{
    struct libevdev *dev = NULL;
    int fd = 1;
    int rc = 1;
    int read_flag = LIBEVDEV_READ_FLAG_NORMAL;

    fd = open(gamepad_portname, O_RDONLY|O_NONBLOCK);
    m_fd = fd;

    rc = libevdev_new_from_fd(m_fd, &dev);
    if (rc < 0) {
        fprintf(stderr, "Failed to init libevdev (%s)\n", strerror(-rc));
        exit(1);
    }
    printf("Input device name: \"%s\"\n", libevdev_get_name(dev));
    printf("Input device ID: bus %#x vendor %#x product %#x\n",
        libevdev_get_id_bustype(dev),
        libevdev_get_id_vendor(dev),
        libevdev_get_id_product(dev));


    // Start of loop
    while(!m_stopThread) {
        struct input_event ev;
        rc = libevdev_next_event(dev, read_flag, &ev);
        if (rc == LIBEVDEV_READ_STATUS_SUCCESS)
            updateGamepad(ev);
        else if (rc == -EAGAIN)
            read_flag = LIBEVDEV_READ_FLAG_NORMAL;
        else if (rc == LIBEVDEV_READ_STATUS_SYNC){
            read_flag = LIBEVDEV_READ_FLAG_SYNC;
            updateGamepad(ev);
        }

        this_thread::sleep_for(chrono::milliseconds(1));

    }
}

/**
 * @brief       Save the new gamepad value into the correct table
 * @param[in]   ev Event indicating a change of gamepad value
 * @retval      void
 * @note        The values of the axes get remapped from [0,255] to [-1,1], 
 *              and the Y-axes inverted so that they point towards the front
 */
void PS5::updateGamepad(input_event ev)
{       
    int value = ev.value;     
    const char* event_type = libevdev_event_type_get_name(ev.type);
    const char* event_code = libevdev_event_code_get_name(ev.type, ev.code);

    // The value gets remapped from [0, 255] to [-1, 1] for axes
    float rescaled_value = 2/(float)MAX_AXIS_VAL * value -1;

    // Lock the mutex
    scoped_lock lock(m_mutex);

    // A button's state has changed
    if (strcmp(event_type, "EV_KEY") == 0){
        if      (strcmp(event_code, "BTN_SELECT") == 0) m_buttons[e_BTN_SELECT] = value;
        else if (strcmp(event_code, "BTN_START") == 0)  m_buttons[e_BTN_START] = value;
        else if (strcmp(event_code, "BTN_MODE") == 0)   m_buttons[e_BTN_MODE] = value;
        else if (strcmp(event_code, "BTN_NORTH") == 0)  m_buttons[e_BTN_NORTH] = value;
        else if (strcmp(event_code, "BTN_WEST") == 0)   m_buttons[e_BTN_WEST] = value;
        else if (strcmp(event_code, "BTN_SOUTH") == 0)  m_buttons[e_BTN_SOUTH] = value;
        else if (strcmp(event_code, "BTN_EAST") == 0)   m_buttons[e_BTN_EAST] = value;
        else if (strcmp(event_code, "BTN_TR") == 0)     m_buttons[e_BTN_TR] = value;
        else if (strcmp(event_code, "BTN_TR2") == 0)    m_buttons[e_BTN_TR2] = value;
        else if (strcmp(event_code, "BTN_TL") == 0)     m_buttons[e_BTN_TL] = value;
        else if (strcmp(event_code, "BTN_TL2") == 0)    m_buttons[e_BTN_TL2] = value;
        else if (strcmp(event_code, "BTN_THUMBR") == 0) m_buttons[e_BTN_THUMBR] = value;
        else if (strcmp(event_code, "BTN_THUMBL") == 0) m_buttons[e_BTN_THUMBL] = value;
    }

    // The value of an absolute axis has changed
    else if (strcmp(event_type, "EV_ABS") == 0){
        if     (strcmp(event_code, "ABS_RX") == 0)    m_axes[e_ABS_RX] =  rescaled_value;
        else if(strcmp(event_code, "ABS_RY") == 0)    m_axes[e_ABS_RY] = -rescaled_value;
        else if(strcmp(event_code, "ABS_X") == 0)     m_axes[e_ABS_X] =   rescaled_value;
        else if(strcmp(event_code, "ABS_Y") == 0)     m_axes[e_ABS_Y] =  -rescaled_value;
        else if(strcmp(event_code, "ABS_Z") == 0)     m_axes[e_ABS_Z] =   rescaled_value;
        else if(strcmp(event_code, "ABS_RZ") == 0)    m_axes[e_ABS_RZ] =  rescaled_value;
        else if(strcmp(event_code, "ABS_HAT0X") == 0) m_buttons[e_ABS_HAT0X] = value;
        else if(strcmp(event_code, "ABS_HAT0Y") == 0) m_buttons[e_ABS_HAT0Y] = -value;
    }
}

}