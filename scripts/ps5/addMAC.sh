#!/bin/sh
# This script adds a udev rule to connect a DualSense PS5 gamepad based on its MAC address
echo -e "This script allows to add a custom udev rule to connect a DualSense PS5 gamepad based on its MAC address if the default rules set by the script \"createRules.sh\" do not work."
echo -e "Make sure to know the MAC address before continuing this script. \n"
sleep 1

while true; do

    echo -e "What is the MAC address of your gamepad? (use lower cases). For example: 10:18:49:4d:a7:18"
    read MAC_ADDRESS

    echo -e "The MAC address you entered is: $MAC_ADDRESS. Is this correct? [y/n]"

    while true; do
        # Read a single character from the input
        read -s -n 1 key

        # Check if the user entered the correct MAc address
        if [[ $key == "y" || $key == "Y" ]] ; then
            break
        elif [[ $key == "n" || $key == "N" ]] ; then
            echo -e
            break
        fi
    done

    # Break the loop if MAC address correct
    if [[ $key == "y" || $key == "Y" ]] ; then
        break
    fi
done

echo -e "Editing the rules file..."
sudo echo "KERNEL==\"event*\", ATTRS{uniq}==\"$MAC_ADDRESS\", ATTRS{properties}==\"0\", SYMLINK+=\"input/event-ps5-gamepad\"" | sudo tee -a /etc/udev/rules.d/ps5-input-events.rules > /dev/null

echo -e "Restarting the rules"
sudo udevadm control --reload-rules && sudo udevadm trigger

echo -e "Udev rules successfully updated! \n"
echo -e "You can now try to connect your gamepad (via cable or bluetooth), and test if the rule works by using \"ls /dev/input\" "
echo -e "If the rule works, you should see an event named \"event-ps5-gamepad\" "
sleep 1