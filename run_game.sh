# Compile the game cheats into a shared object
g++ cheats/team6_cheats.cpp -shared -fPIC -o cheats/cheats.so

# Start game and use cheats
LD_PRELOAD=cheats/cheats.so ./PwnAdventure3-Linux-Shipping
