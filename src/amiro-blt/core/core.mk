################################################################################
# AMiRo-OS is an operating system designed for the Autonomous Mini Robot       #
# (AMiRo) platform.                                                            #
# Copyright (C) 2016..2019  Thomas Schöpping et al.                            #
#                                                                              #
# This program is free software: you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation, either version 3 of the License, or            #
# (at your option) any later version.                                          #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program.  If not, see <http://www.gnu.org/licenses/>.        #
#                                                                              #
# This research/work was supported by the Cluster of Excellence Cognitive      #
# Interaction Technology 'CITEC' (EXC 277) at Bielefeld University, which is   #
# funded by the German Research Foundation (DFG).                              #
################################################################################



# absolute path to this directory
AMIROOS_CORE_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

# include paths
AMIROOSCOREINC = $(AMIROOS_CORE_DIR)inc

# C source files
AMIROOSCORECSRC = $(AMIROOS_CORE_DIR)src/aos_debug.c \
                  $(AMIROOS_CORE_DIR)src/aos_iostream.c \
                  $(AMIROOS_CORE_DIR)src/aos_shell.c \
                  $(AMIROOS_CORE_DIR)src/aos_system.c \
                  $(AMIROOS_CORE_DIR)src/aos_thread.c \
                  $(AMIROOS_CORE_DIR)src/aos_time.c \
                  $(AMIROOS_CORE_DIR)src/aos_timer.c \
                  $(AMIROOS_CORE_DIR)src/aos_unittest.c

# C++ source files
AMIROOSCORECPPSRC = $(AMIROOS_CORE_DIR)src/aos_main.cpp

