# ArmA 3 Unix Launcher install script for Mac OS X

This scripts assists in installation of ArmA 3 Unix Launcher on Mac OS X.

What the script does:
1. Check if git is installed - if no - opens download page.
2. Check if Homebrew if installed - if no - installs it.
3. Check if Gtkmm3 is installed - as above.
4. Clones this git repository's master branch (the code) and builds it.
5. Moves arma3-unix-launcher.app into /Applications/

After running this script you can find the launcher in Applications directory.