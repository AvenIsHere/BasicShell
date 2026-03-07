## BShell
This is a shell program written in C++ for POSIX-compliant operating systems.

This was initially made as coursework for university, however, I have since developed it considerably.
### Licence
The source code for this project is licensed under the GNU General Public License v3.0.
## Installation
### Prerequisites
* Linux (Debian/Ubuntu): `sudo apt install build-essential cmake libreadline-dev`
* MacOS: `brew install cmake readline`
### Installation
```shell
git clone https://github.com/AvenIsHere/BShell.git
cd BShell
mkdir build && cd build
cmake ..
make
```
## Usage
To use the shell, run the following command from the directory you build it in:
```shell
./BShell
```