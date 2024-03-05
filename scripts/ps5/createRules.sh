#!/bin/sh
# This script creates the basic udev rule to assign a custom name to a PS5 gamepad when it connects

echo -e "Creating the basic udev rule for DualSense PS5 gamepads... \n"
sleep 1

sudo bash -c 'cat > /etc/udev/rules.d/ps5-input-events.rules' << EOF
KERNEL=="event*" , SUBSYSTEM=="input", MODE="0666"
KERNEL=="event*", ATTRS{name}=="DualSense Wireless Controller", SYMLINK+="input/event-ps5-gamepad"
KERNEL=="event*", ATTRS{name}=="Sony Interactive Entertainment DualSense Wireless Controller", SYMLINK+="input/event-ps5-gamepad"
EOF

# Restart the udev rules
sudo udevadm control --reload-rules && sudo udevadm trigger

echo -e "Udev rules successfully created! \n"
echo -e "You can now try to connect your gamepad (via cable or bluetooth), and test if the rule works by using \"ls /dev/input\" "
echo -e "If the rule works, you should see an event named \"event-ps5-gamepad\" "
echo -e "If not, use the script to add a rule based on the gamepad's MAC address"
sleep 1