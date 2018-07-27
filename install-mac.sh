#!/bin/bash

OS="`uname`"
if [[ "$OS" != "Darwin" ]]; then
	echo "This script works only on Mac OS X"
	exit 1
fi

echo "Checking for git"
if [ ! -f "`which git`" ]; then
	echo "Git not found"
	open https://git-scm.com/download/mac
	exit 1
fi

echo "Checking for Homebrew installation..."
if [ ! -f "`which brew`" ]; then
	echo "Homebrew not found! Starting installation"
	ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
fi

echo "Checking for pkg-config"
if brew ls --versions pkg-config > /dev/null; then
	echo "pkg-config is installed"
else
	echo "Installing pkg-config"
	brew install pkg-config
fi

echo "Checking for Gtkmm3"
if brew ls --versions gtkmm3 > /dev/null; then
	echo "Gtkmm3 is installed"
else
	echo "Installing Gtkmm3"
	brew install gtkmm3
fi

echo "Checking for CMake"
if brew ls --versions cmake > /dev/null; then
	echo "CMake is installed"
else
	echo "Installing CMake"
	brew install cmake
fi

rm -rf a3unixlauncher
mkdir a3unixlauncher
cd a3unixlauncher

echo "Cloning git repository"

git clone https://github.com/muttleyxd/arma3-unix-launcher.git > /dev/null
cd arma3-unix-launcher
mkdir build
cd build
cmake ..
make
echo "ArmA 3 Unix Launcher built successfully. Moving it into /Applications/"
rm -rf /Applications/arma3-unix-launcher.app
mv arma3-unix-launcher.app /Applications/arma3-unix-launcher.app
