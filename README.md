# KMR_gamepads

Library for handling inputs from different gamepads, through the evdev interface.

Dependencies:
- libevdev

How to use: go to the KMR_gamepads folder, then:
```bash
mkdir build
cd build
cmake ../
cmake --build .
```

If you have Doxygen and Graphviz installed, you can regenerate the documentation locally with
```make docs``` from the `build` folder after the cmake.
The generated documentation can be found in docs/generated_docs/html.
