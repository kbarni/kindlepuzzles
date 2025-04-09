# Kindle port of Simon Tatham's Puzzle collection

Simon wrote this collection because he thought there should be more small desktop toys available: little games you can pop up in a window and play for two or three minutes while you take a break from whatever else you were doing. And he was also annoyed that every time he found a good game on (say) Unix, it wasn't available the next time he was sitting at a Windows machine, or vice versa; so I arranged that everything in his personal puzzle collection will happily run on both those platforms and more. When he finds (or perhaps invents) further puzzle games that he likes, they'll be added to this collection and will immediately be available on both platforms. And if anyone feels like writing any other front ends for platforms it doesn't support (which people already have) then all the games in this framework will immediately become available on another platform as well. 

Original webpage and more info here: https://www.chiark.greenend.org.uk/~sgtatham/puzzles/

## Building for Kindle

git clone 
cd kindlepuzzles
mkdir build
cd build
cmake .. -DPUZZLES_GTK_VERSION=2
make

## Remarks for Kindle version

I find that this collection is well suited for playing on Kindle.

Some games however won't work or are difficult to play. Some need keyboard and some need right click to play correctly.

*Note:* Work on adding a keyboard is already on its way, but currently the window doesn't resize itself correctly.
