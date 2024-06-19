# Neural-Cellular-Automata

--INFO--

An interface to create Neural cellular automatas easily

C language

libraries : raylib


--HOW TO USE--

Execute nca.exe to launch (if it doesn't work compile yourself the file "nca.c")

The grid size can be changed by pressing "+" or "-"

Press "r" to reset and randomize the screen with float values between 0 and 1

Press "c" to clear everything

Press "g" to switch on or off the grid

Press "h" to hide three frames out of four, this helps to iteration speed and reduces flickering automatons

Press Space to pause/resume

Press "s" to step one single iteration

Use Left and Right click to draw and delete cells


--Left UI HOW TO USE--

Each cell in the grid is a float value between 0 and 1.
The 3*3 grid in the top left corner is a representation of the neighbors of a cell. For each iteration, each cell of the global grid looks at its neighbors values, multiply them by the corresponding values (in the top left display) and adds them all together. The result is then given to the activation function (the function in the bottom left corner) and the result is the final value given to the cell in the next iteration.

Modify the values of the neighbors grid by left or right clicking on the display, respectively to increase or decrease the value. Clicking in the left half of a cell will change the value by 0.1 and clicking in the right half will change the value by 0.01.

To change the activation function, you will have to manually modify in the code the function called "activation" (Temporary).
