#!/bin/bash

aslib_include_path="/usr/local/include/aslib"
aslib_lib_path="/usr/local/lib"

# check if aslib include directory exists,if so, remove it
if [ -d "$aslib_include_path" ]; then
    echo "Removing existing Aslib include directory..."
    sudo rm -rf "$aslib_include_path"
fi
# check whether there are the two files aslib.a and aslib.so in the lib directory.
# if so, remove them aslib.a and aslib.so
if [ -d "$aslib_lib_path/aslib.a" || -d "$aslib_lib_path/aslib.so" ]; then
    echo "Removing existing Aslib library directory..."
    sudo rm -rf "$aslib_lib_path/aslib.a" "$aslib_lib_path/aslib.so"
fi

# reinstall aslib
echo "Installing Aslib..."
sudo mkdir -p "$aslib_include_path"
sudo mkdir -p "$aslib_lib_path"

# clean previous build files
make clean

# exectue the make cmd
make 

# check if make was successful
if [ $? -eq 0 ]; then
    echo "Aslib installed successfully."
else
    echo "Failed to install Aslib."
    exit 1
fi

# copy the include files
sudo cp -r include/* "$aslib_include_path"
# copy the library files
sudo cp -r lib/* "$aslib_lib_path"
echo "Aslib files copied successfully."
# update the library cache
sudo ldconfig
echo "Library cache updated successfully."
