## Repository for UCR RoboSub software

This repository contains the in-progress work of the UCR RoboSub Software Team, 
and is intended to facilitate remote work within the team between in-person 
meetings. New modules will be added and removed regularly as work progresses on 
them, and nothing contained within should be considered as complete.

Each module is seperated into it's own subfolders and contains it's own Makefile
and/or CMakeLists.txt. As modules are determined to be stable they will be moved
to a seperate repository which will contain only working code to be tested on 
the sub itself.

## Requirements

Individual requirements for the modules will be detailed within their respective
readmes. Global requirements are for a simple development environment including 
a C++ compiler, make, and cmake. Within a Linux environment, these can be 
installed with the following commands:

Debian/Ubuntu
`$ sudo apt-get install build-essentials make cmake`

Fedora/Red Hat
`$ sudo yum install build-essentials make cmake`

If you prefer using an IDE, such as Microsoft Visual, you are welcome to. Keep 
in mind that any bloated files that are specific to your environment may be 
purged from the repository. This is done to maintain as uniform a folder 
structure between modules as possible, and may break IDE support after a pull 
request.
