## Vision

The automation of tasks required of RoboSub lean heavily on computer vision, 
making this one of the most important modules. Each folder here contains a 
seperate attempt at aquiring a target based on different parameters. These are 
meant to be varied attempts at solving the problem here, and should not include 
dependencies between each other if possible.

## Requirements

These modules require OpenCV and some additional modules that are not a part of 
the core installation.

# OpenCV

You can install OpenCV from <http://opencv.org>

# OpenCV (Additional Modules)

The additional OpenCV functionality required by some of these modules can be 
installed from the opencv\_contrib repository found at <https://github.com/opencv/opencv_contrib>

## Overview

- **colortracking**: A rudimentory method of tracking an object based on HSV 
values. This is a good place to start if you are learning OpenCV, but is not 
very useful for our applications in RoboSub.

- **saliency**: Attempt at identifying salient regions of a video capture using opencv\_contrib saliency support.
