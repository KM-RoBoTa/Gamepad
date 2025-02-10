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
 * @note        By default, open Sony Dualsense gamepad's custom event name set by udev rules created
 *              by the scripts. If you wish to use a different gamepad, use the overload of this cstr
 */
PS5::PS5()
{
    m_stopThread = false;

    // Initialize vectors
    m_buttons = vector<int>(NBR_BUTTONS, 0);
    m_axes = vector<float>(NBR_AXES, 0);

    m_ps5_thread = thread(&KMR::gamepads::PS5::gamepadLoop, this, "/dev/input/event-ps5-gamepad");
}


/**
 * @brief       Create and initialize a PS5 object with a different gamepad than the default
 * @param[in]   gamepad_portname Port handling the gamepad, in the form of "/dev/input/eventX"
 */
PS5::PS5(const char* gamepad_portname)
{
    m_stopThread = false;

    // Initialize vectors
    m_buttons = vector<int>(NBR_BUTTONS, 0);
    m_axes = vector<float>(NBR_AXES, 0);

    m_ps5_thread = thread(&KMR::gamepads::PS5::gamepadLoop, this, gamepad_portname);
}


/**
 * @brief   Class destructor. Takes care of safely stopping the thread
 */
PS5::~PS5()
{
    m_stopThread = true;

    // Main thread waits until the PS5 thread finishes
    if (m_ps5_thread.joinable()) 
        m_ps5_thread.join();

    cout << "Gamepad's thread is safely stopped" << endl;
}


/**
 * @brief       Read and save the current gamepad values. To loop in a thread
 * @param[in]   gamepad_portname Port handling the gamepad, in the form of /dev/input/eventX
 */
void PS5::gamepadLoop(const char* gamepad_portname)
{
    struct libevdev *dev = NULL;
    int read_flag = LIBEVDEV_READ_FLAG_NORMAL;

    int fd = open(gamepad_portname, O_RDONLY|O_NONBLOCK);

    int rc = libevdev_new_from_fd(fd, &dev);
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
    cout << "Gamepad monitoring starting" << endl;
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

    close(fd);
}

/**
 * @brief       Save the new gamepad value into the correct table
 * @param[in]   ev Event indicating a change of gamepad value
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
        if      (strcmp(event_code, "BTN_SELECT") == 0) m_buttons[SELECT_BTN] = value;
        else if (strcmp(event_code, "BTN_START") == 0)  m_buttons[START_BTN] = value;
        else if (strcmp(event_code, "BTN_MODE") == 0)   m_buttons[PS_BTN] = value;
        else if (strcmp(event_code, "BTN_NORTH") == 0)  m_buttons[TRIANGLE_BTN] = value;
        else if (strcmp(event_code, "BTN_WEST") == 0)   m_buttons[SQUARE_BTN] = value;
        else if (strcmp(event_code, "BTN_SOUTH") == 0)  m_buttons[CROSS_BTN] = value;
        else if (strcmp(event_code, "BTN_EAST") == 0)   m_buttons[CIRCLE_BTN] = value;
        else if (strcmp(event_code, "BTN_TR") == 0)     m_buttons[R1_BTN] = value;
        else if (strcmp(event_code, "BTN_TR2") == 0)    m_buttons[R2_BTN] = value;
        else if (strcmp(event_code, "BTN_TL") == 0)     m_buttons[L1_BTN] = value;
        else if (strcmp(event_code, "BTN_TL2") == 0)    m_buttons[L2_BTN] = value;
        else if (strcmp(event_code, "BTN_THUMBR") == 0) m_buttons[R3_BTN] = value;
        else if (strcmp(event_code, "BTN_THUMBL") == 0) m_buttons[L3_BTN] = value;
    }

    // The value of an absolute axis has changed
    else if (strcmp(event_type, "EV_ABS") == 0){
        if     (strcmp(event_code, "ABS_RX") == 0)    m_axes[RSTICK_X] =  rescaled_value;
        else if(strcmp(event_code, "ABS_RY") == 0)    m_axes[RSTICK_Y] = -rescaled_value;
        else if(strcmp(event_code, "ABS_X") == 0)     m_axes[LSTICK_X] =  rescaled_value;
        else if(strcmp(event_code, "ABS_Y") == 0)     m_axes[LSTICK_Y] = -rescaled_value;
        else if(strcmp(event_code, "ABS_Z") == 0)     m_axes[L2_AXIS] =   rescaled_value;
        else if(strcmp(event_code, "ABS_RZ") == 0)    m_axes[R2_AXIS] =   rescaled_value;
        else if(strcmp(event_code, "ABS_HAT0X") == 0) {
            switch (value)
            {
            case 0:
                m_buttons[LEFT_BTN] = 0;
                m_buttons[RIGHT_BTN] = 0;
                break;
            case 1:
                m_buttons[LEFT_BTN] = 0;
                m_buttons[RIGHT_BTN] = 1;
                break;        
            case -1:
                m_buttons[LEFT_BTN] = 1;
                m_buttons[RIGHT_BTN] = 0;
                break;       
            default:
                break;
            }
        }
        else if(strcmp(event_code, "ABS_HAT0Y") == 0) {
            switch (value)
            {
            case 0:
                m_buttons[UP_BTN] = 0;
                m_buttons[DOWN_BTN] = 0;
                break;
            case 1:
                m_buttons[UP_BTN] = 0;
                m_buttons[DOWN_BTN] = 1;
                break;        
            case -1:
                m_buttons[UP_BTN] = 1;
                m_buttons[DOWN_BTN] = 0;
                break;       
            default:
                break;
            }
        }       
    }
}

/**
 * @brief       Get the inputs for the query buttons
 * @param[in]   buttons Query buttons 
 * @return      Inputs of the query buttons
 */
vector<int> PS5::getBulkButtons(vector<ps5Buttons> buttons)
{
    int nbrButtons = buttons.size();
    vector<int> inputs(nbrButtons, 0);

    // Grab the gamepad values from the internal storage
    {
        // Lock the mutex
        scoped_lock lock(m_mutex);    
        for (int i=0; i<nbrButtons; i++)
            inputs[i] = m_buttons[buttons[i]];
    }

    return inputs;
}

/**
 * @brief       Get the input for the query button
 * @param[in]   button Query button
 * @return      Input of the query button
 */
int PS5::getButton(ps5Buttons button)
{
    scoped_lock lock(m_mutex);    
    return(m_buttons[button]);
}

/**
 * @brief       Get the inputs for the query axes
 * @param[in]   axes Query axes
 * @return      Inputs of the query axes
 */
vector<float> PS5::getBulkAxes(std::vector<ps5Axes> axes)
{
    int nbrAxes = axes.size();
    vector<float> inputs(nbrAxes, 0);

    // Grab the gamepad values from the internal storage
    {
        // Lock the mutex
        scoped_lock lock(m_mutex);    
        for (int i=0; i<nbrAxes; i++)
            inputs[i] = m_axes[axes[i]];
    }

    return inputs;
}

/**
 * @brief       Get the input for the query axis
 * @param[in]   axis Query axis
 * @return      Input of the query axis
 */
float PS5::getAxis(ps5Axes axis)
{
    scoped_lock lock(m_mutex); 
    return(m_axes[axis]);
}


}