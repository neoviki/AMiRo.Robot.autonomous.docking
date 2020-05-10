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

#!/bin/bash

################################################################################
# GENERIC FUNCTIONS                                                            #
################################################################################

### print an error message #####################################################
# Prints a error <message> to standard output.
#If variable 'LOG_FILE' is specified, the message is also appended to the given file.
#
# usage:      printError <message>
# arguments:  <message>
#                 Message string to print.
# return:     n/a
#
function printError {
  local string="ERROR:   $1"
  # if a log file is specified
  if [ -n "$LOG_FILE" ]; then
    printf "[$(date '+%Y-%m-%d %H:%M:%S')] $string" >> $LOG_FILE
  fi
  printf "$(tput setaf 1)>>> $string$(tput sgr 0)" 1>&2
}

### print a warning message ####################################################
# Prints a warning <message> to standard output.
#If variable 'LOG_FILE' is specified, the message is also appended to the given file.
#
# usage:      printMessage <message>
# arguments:  <message>
#                 Message string to print.
# return:     n/a
#
function printWarning {
  local string="WARNING: $1"
  # if a log file is specified
  if [ -n "$LOG_FILE" ]; then
    printf "[$(date '+%Y-%m-%d %H:%M:%S')] $string" >> $LOG_FILE
  fi
  printf "$(tput setaf 3)>>> $string$(tput sgr 0)"
}

### print an information message ###############################################
# Prints an information <message> to standard output.
#If variable 'LOG_FILE' is specified, the message is also appended to the given file.
#
# usage:      printInfo <message>
# arguments:  <message>
#                 Message string to print.
# return:     n/a
#
function printInfo {
  local string="INFO:    $1"
  # if a log file is specified
  if [ -n "$LOG_FILE" ]; then
    printf "[$(date '+%Y-%m-%d %H:%M:%S')] $string" >> $LOG_FILE
  fi
  printf "$(tput setaf 2)>>> $string$(tput sgr 0)"
}

### print a message to file ####################################################
# Appends a <message> to a log file, specified by the variable 'LOG_FILE'.
#
# usage       printLog <message>
# arguments:  <message>
#                 Message string to print.
# return:     n/a
#
function printLog {
  local string="LOG:     $1"
  # if a log file is specified
  if [ -n "$LOG_FILE" ]; then
    printf "[$(date '+%Y-%m-%d %H:%M:%S')] $string" >> $LOG_FILE
  fi
}

### exit the script normally ###################################################
# Prints a delimiter and exits the script normally (returns 0).
#
# usage:      quitScript
# arguments:  n/a
# return:     0
#                 No error or warning occurred.
#
function quitScript {
  printInfo "exiting $(realpath ${BASH_SOURCE[0]})\n"
  printf "\n"
  printf "######################################################################\n"
  exit 0
}

### read a user input ##########################################################
# Reads a single character user input from a set up <options> and stores it in
# a given <return> variable.
#
# usage:      readUserInput <options> <return>
# arguments:  <options>
#                 String definiing the set of valid characters.
#                 If the string is empty, the user can input any character.
#             <return>
#                 Variable to store the selected character to.
# return:     n/a
#
function readUserInput {
  local input=""
  # read user input
  while [ -z $input ] || ( [ -n "$1" ] && [[ ! $input =~ ^[$1]$ ]] ); do
    read -p "your selection: " -n 1 -e input
    if [ -z $input ] || ( [ -n "$1" ] && [[ ! $input =~ ^[$1]$ ]] ); then
      printWarning "[$input] is no valid action\n"
    fi
  done
  printLog "[$input] has been selected\n"
  eval $2="$input"
}

### check whether argument is an option ########################################
# Checks a <string> whether it is an option.
# Options are defined to either start with '--' followed by any string, or
# to start with a single '-' followed by a single character, or
# to start with a single '-' followed by a single character, a '=' and any string.
# Examples: '--option', '--option=arg', '-o', '-o=arg', '--'
#
# usage:      parseIsOption <string>
# arguments:  <string>
#                 A string to check whether it is an option.
# return:     0
#                 <string> is an option.
#             -1
#                 <string> is not an option.
#
function parseIsOption {
  if [[ "$1" =~ ^-(.$|.=.*) ]] || [[ "$1" =~ ^--.* ]]; then
    return 0
  else
    return -1
  fi
}

### set the log file ###########################################################
# Sets a specified <infile> as log file and checks whether it already exists.
# If so, the log may either be appended to the file, its content can be cleared,
# or no log is generated at all.
# The resulting path is stored in <outvar>.
#
# usage:      setLogFile [--option=<option>] [--quiet] <infile> <outvar>
# arguments:  --option=<option>
#                 Select what to do if <file> already exists.
#                 Possible values are 'a', 'c', 'r' and 'n'.
#                 - a: append (starts with a separator)
#                 - c: continue (does not insert a seperator)
#                 - r: delete and restart
#                 - n: no log
#                 If no option is secified but <file> exists, an interactive selection is provided.
#             --quiet
#                 Suppress all messages.
#             <infile>
#                 Path of the wanted log file.
#             <outvar>
#                 Variable to store the path of the log file to.
# return:     0
#                 No error or warning occurred.
#             -1
#                 Error: invalid input
#
function setLogFile {
  local filepath=""
  local option=""
  local quiet=false

  # parse arguments
  local otherargs=()
  while [ $# -gt 0 ]; do
    if ( parseIsOption $1 ); then
      case "$1" in
        -o=*|--option=*)
          option=${1#*=}; shift 1;;
        -o*|--option*)
          option="$2"; shift 2;;
        -q|--quiet)
          quiet=true; shift 1;;
        *)
          printError "invalid option: $1\n"; shift 1;;
      esac
    else
      otherargs+=("$1")
      shift 1
    fi
  done
  filepath=$(realpath ${otherargs[0]})

  # if file already exists
  if [ -e $filepath ]; then
    # if no option was specified, ask what to do
    if [ -z "$option" ]; then
      printWarning "log file $filepath already esists\n"
      local userinput=""
      printf "Select what to do:\n"
      printf "  [A] - append log\n"
      printf "  [R] - restart log (delete existing file)\n"
      printf "  [N] - no log\n"
      readUserInput "AaRrNn" userinput
      option=${userinput,,}
    fi
    # evaluate option
    case "$option" in
      a|c)
        if [ $quiet = false ]; then
          printInfo "appending log to $filepath\n"
        fi
        if [ $option != c ]; then
          printf "\n" >> $filepath
          printf "######################################################################\n" >> $filepath
          printf "\n" >> $filepath
        fi
        ;;
      r)
        echo -n "" > $filepath
        if [ $quiet = false ]; then
          printInfo "content of $filepath wiped\n"
        fi
        ;;
      n)
        if [ $quiet = false ]; then
          printInfo "no log file will be generated\n"
        fi
        filepath=""
        ;;
      *) # sanity check (return error)
        printError "unexpected argument: $option\n"; return -1;;
    esac
  else
    if [ $quiet = false ]; then
      printInfo "log file set to $filepath\n"
    fi
  fi

  eval ${otherargs[1]}="$filepath"

  return 0
}

################################################################################
# SPECIFIC FUNCTIONS                                                           #
################################################################################

### print welcome text #########################################################
# Prints a welcome message to standard out.
#
# usage:      printWelcomeText
# arguments:  n/a
# return:     n/a
#
function printWelcomeText {
  printf "######################################################################\n"
  printf "#                                                                    #\n"
  printf "#                  Welcome to the QtCreator setup!                   #\n"
  printf "#                                                                    #\n"
  printf "######################################################################\n"
  printf "#                                                                    #\n"
  printf "# Copyright (c) 2016..2019  Thomas Schöpping                         #\n"
  printf "#                                                                    #\n"
  printf "# This is free software; see the source for copying conditions.      #\n"
  printf "# There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR  #\n"
  printf "# A PARTICULAR PURPOSE. The development of this software was         #\n"
  printf "# supported by the Excellence Cluster EXC 227 Cognitive Interaction  #\n"
  printf "# Technology. The Excellence Cluster EXC 227 is a grant of the       #\n"
  printf "# Deutsche Forschungsgemeinschaft (DFG) in the context of the German #\n"
  printf "# Excellence Initiative.                                             #\n"
  printf "#                                                                    #\n"
  printf "######################################################################\n"
}

### print help #################################################################
# Prints a help text to standard out.
#
# usage:      printHelp
# arguments:  n/a
# return:     n/a
#
function printHelp {
  printInfo "printing help text\n"
  printf "usage:    $(basename ${BASH_SOURCE[0]}) [-h|--help] [--module=<module>] [-a|--all] [-c|--clean] [-w|--wipe] [-q|--quit] [--log=<file>]\n"
  printf "\n"
  printf "options:  -h, --help\n"
  printf "              Print this help text.\n"
  printf "          --module=<module>\n"
  printf "              Create project for a single module.\n"
  printf "          -a, --all\n"
  printf "              Create projects for all modules.\n"
  printf "          -c, --clean\n"
  printf "              Delete project files.\n"
  printf "          -w, --wipe\n"
  printf "              Delete project and .user files.\n"
  printf "          -q, --quit\n"
  printf "              Quit the script.\n"
  printf "          --log=<file>\n"
  printf "              Specify a log file.\n"
}

### read directory where to create/delete projects #############################
# Read the directory where to create/delete project files from user.
#
# usage:      getProjectDir <pathvar>
# arguments:  <pathvar>
#                 Variable to store the selected path to.
# return:     n/a
#
function getProjectDir {
  printLog "reading path for project files from user...\n"
  local amiroosdir=$(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../)
  local input=""
  read -p "Path where to create/delete project files: " -i $amiroosdir -e input
  printLog "user selected path $(realpath $input)\n"
  eval $1="$(realpath $input)"
}

### retrieves the ARM-NONE-EABI-GCC include directory ##########################
# Retrieves the include directory of the currently set arm-none-eabi-gcc.
#
# usage:      retrieveGccIncludeDir <path>
# arguments:  <path>
#                 Variable to store the path to.
# return:    0
#                 No error or warning occurred.
#            -1
#                 Error: Command 'arm-none-eabi-gcc' not found.
#
function retrieveGccIncludeDir {
  # retrieve binary path or link
  local binpath=$(which arm-none-eabi-gcc)
  if [ -z "$binpath" ]; then
    printError "command 'arm-none-eabi-gcc' not found\n"
    return -1
  else 

    # traverse any links
    while [ -L "$binpath" ]; do
      binpath=$(realpath $(dirname $binpath)/$(readlink $binpath))
    done
    printInfo "gcc-arm-none-eabi detected: $binpath\n"

    # return include path
    eval $1=$(realpath $(dirname ${binpath})/../arm-none-eabi/include/)

    return 0
  fi
}

### detect available modules ###################################################
# Detect all avalable modules supported by AMiRo-OS.
#
# usage:      detectModules <modulearray>
# arguments:  <modulearray>
#                 Array variable to store all detected modules to.
# return:     n/a
#
function detectModules {
  local modulesdir=$(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../modules)
  local modules_detected=()

  # detect all available modules (via directories)
  for dir in $(ls -d ${modulesdir}/*/); do
    modules_detected[${#modules_detected[@]}]=$(basename $dir)
  done

  # set the output variable
  eval "$1=(${modules_detected[*]})"
}

### create project files for a single module ###################################
# Create project files for a single module.
#
# usage:      createModuleProject <modules> [-m|--module=<module>] [-p|--path=<path>] [--gcc=<path>] [-o|--out=<var>] [--gccout=<var>]
# arguments:  <modules>
#                 Array containing all modules available.
#             -m, --module <module>
#                 Name (folder name) of the module for which project files shall be generated.
#             -p, --path <path>
#                 Path where to create the project files.
#             --gcc=<path>
#                 Path to the GCC include directory.
#             -o, --out <var>
#                 Variable to store the path to.
#             --gccout=<var>
#                 Variable to store the path to the GCC include directory to.
#                 If this optional arguments is absent, ths function will ask for user input.
# return:     0
#                 No error or warning occurred.
#             1
#                 Aborted by user.
#             -1
#                 No modules available.
#             -2
#                 The specified <module> could not be found.
#             -3
#                 Parsing the project for the specified module failed.
#
function createModuleProject {
  local userdir=$(pwd)
  local modulesdir=$(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../modules)
  local modules=("${!1}")
  local module=""
  local moduleidx=""
  local projectdir=""
  local gccincludedir=""
  local outvar=""
  local gccoutvar=""

  # parse arguments
  local otherargs=()
  while [ $# -gt 0 ]; do
    if ( parseIsOption $1 ); then
      case "$1" in
        -m=*|--module=*)
          module="${1#*=}"; shift 1;;
        -m|--module)
          module="$2"; shift 2;;
        -p=*|--path=*)
          projectdir=$(realpath "${1#*=}"); shift 1;;
        -p|--path)
          projectdir=$(realpath "$2"); shift 2;;
        --gcc=*)
          gccincludedir=$(realpath "${1#*=}"); shift 1;;
        --gcc)
          gccincludedir=$(realpath "$2"); shift 2;;
        -o=*|--out=*)
          outvar=${1#*=}; shift 1;;
        -o|--out)
          outvar=$2; shift 2;;
        --gccout=*)
          gccoutvar=$(realpath "${1#*=}"); shift 1;;
        --gccout)
          gccoutvar=$(realpath "$2"); shift 2;;
        *)
          printError "invalid option: $1\n"; shift 1;;
      esac
    else
      otherargs+=("$1")
      shift 1
    fi
  done

  # sanity check for the modules variable
  if [ -z "${modules[*]}" ]; then
    printError "no modules available\n"
    return -1
  fi

  # select module
  if [ -z $module ]; then
    # list all available modules
    printInfo "choose a module or type 'A' to abort:\n"
    for (( idx=0; idx<${#modules[@]}; ++idx )); do
      printf "%4u: %s\n" $(($idx + 1)) "${modules[$idx]}"
    done
    # read user input
    printLog "read user selection\n"
    local userinput=""
    while [[ ! "$userinput" =~ ^[0-9]+$ ]] || [ ! "$userinput" -gt 0 ] || [ ! "$userinput" -le ${#modules[@]} ] && [[ ! "$userinput" =~ ^[Aa]$ ]]; do
      read -p "your selection: " -e userinput
      printLog "user selection: $userinput\n"
      if [[ ! "$userinput" =~ ^[0-9]+$ ]] || [ ! "$userinput" -gt 0 ] || [ ! "$userinput" -le ${#modules[@]} ] && [[ ! "$userinput" =~ ^[Aa]$ ]]; then
        printWarning "Please enter an integer between 1 and ${#modules[@]} or 'A' to abort.\n"
      fi
    done
    if [[ "$userinput" =~ ^[Aa]$ ]]; then
      printWarning "aborted by user\n"
      return 1
    fi
    # store selection
    moduleidx=$(($userinput - 1))
    module="${modules[$moduleidx]}"
    printf "\n"
  else
    # search all modules for the selected one
    for (( idx=0; idx<${#modules[@]}; ++idx )); do
      if [ "${modules[$idx]}" = "$module" ]; then
        moduleidx=$idx
        break
      fi
    done
    # error if the module could not be found
    if [ -z $moduleidx ]; then
      printError "module ($module) not available\n"
      return -2
    fi
  fi

  # read absolute project directory if required
  if [ -z "$projectdir" ]; then
    getProjectDir projectdir
    printf "\n"
  fi

  # check for existing project files
  local projectfiles="$(find ${projectdir} -maxdepth 1 -type f | grep -E "${module}\.(includes|files|config|creator)$")"
  IFS=$'\n'; projectfiles=($projectfiles); unset IFS
  if [ ! -z "${projectfiles[*]}" ]; then
    printWarning "The following files will be overwritten:\n"
    for pfile in ${projectfiles[@]}; do
      printWarning "\t$(basename $pfile)\n"
    done
    local userinput=""
    printInfo "Continue and overwrite? [y/n]\n"
    readUserInput "YyNn" userinput
    case "$userinput" in
      Y|y)
        ;;
      N|n)
        printWarning "Project generation for ${module} module aborted by user\n"
        return 1
        ;;
      *)
        printError "unexpected input: ${userinput}\n"; return -999;;
    esac
    printf "\n"
  fi

  # print message
  printInfo "generating QtCreator project files for the $module module...\n"

  # retrieve absolute GCC include path
  if [ -z "$gccincludedir" ]; then
    retrieveGccIncludeDir gccincludedir
  fi

  # change to project directory
  cd "$projectdir"

  # run make, but only run the GCC preprocessor and produce no binaries
  local amiroosrootdir=$(realpath $(dirname ${BASH_SOURCE[0]})/../../..)
  local sourcefiles=()
  local sourcefile=""
  local parse_state="WAIT_FOR_INCLUDE_OR_COMPILE"
  # capture all output from make and GCC and append the return value of make as last line
  printInfo "processing project (this may take a while)...\n"
  local rawout=$(make --directory ${amiroosrootdir}/modules/${module} --always-make USE_OPT="-v -E -H" USE_VERBOSE_COMPILE="no" OUTFILES="" 2>&1 && echo $?)
  # check whether the make call was successfull
  if [[ $(echo "${rawout}" | tail -n 1) != "0" ]]; then
    printError "executing 'make' in module directory failed\n"
    cd "$userdir"
    return -3
  else
    # cleanup
    make --directory ${amiroosrootdir}/modules/${module} clean &>/dev/null
  fi
  # extract file names from raw output
  IFS=$'\n'; rawout=($rawout); unset IFS
  for line in "${rawout[@]}"; do
    case $parse_state in
      WAIT_FOR_INCLUDE_OR_COMPILE)
        # lines stating included files look like:
        # ... <../relative/path/to/file>
        if [[ "$line" =~ ^\.+[[:blank:]].+\..+$ ]]; then
          sourcefile=${line##* }
          if [[ ! "$sourcefile" =~ ^/ ]]; then
            sourcefile=$(realpath ${amiroosrootdir}/modules/${module}/${sourcefile})
          fi
          sourcefiles[${#sourcefiles[@]}]="$sourcefile"
        # whenever the next source file is processed, a message appears like:
        # Compining <filnemame>
        elif [[ "$line" =~ ^Compiling[[:blank:]].+\..+$ ]]; then
          printf "."
          sourcefile=${line##* }
          parse_state="WAIT_FOR_COMPILERCALL"
        fi;;
      WAIT_FOR_COMPILERCALL)
        # wait for the actual call of the compiler to retrieve the full path to the source file
        if [[ "$line" == *${sourcefile}* ]]; then
          line="${line%%${sourcefile}*}${sourcefile}"
          sourcefile=${line##* }
          sourcefile=$(realpath ${amiroosrootdir}/modules/${module}/${line##* })
          sourcefiles[${#sourcefiles[@]}]="$sourcefile"
          parse_state="WAIT_FOR_INCLUDE_OR_COMPILE"
        fi;;
    esac
  done
  unset rawout
  printf "\n"
  # sort and remove duplicates
  IFS=$'\n'; sourcefiles=($(sort --unique <<< "${sourcefiles[*]}")); unset IFS

  # extract include paths
  local includes=()
  for source in ${sourcefiles[*]}; do
    includes[${#includes[@]}]="$(dirname ${source})"
  done
  # sort and remove duplicates
  IFS=$'\n'; includes=($(sort --unique <<< "${includes[*]}")); unset IFS

  # generate the .files file, containing all source files
  echo "" > ${projectdir}/${module}.includes
  for inc in ${includes[*]}; do
    echo "$inc" >> ${projectdir}/${module}.includes
  done
  # generate the .incldues file, containing all include paths
  echo "" > ${projectdir}/${module}.files
  for source in ${sourcefiles[*]}; do
    # skip GCC files
    if [[ ! "$source" =~ .*/gcc.* ]]; then
      echo "$source" >> ${projectdir}/${module}.files
    fi
  done
  # generate a default project configuration file if it doesn't exist yet
  if [ ! -f ${projectdir}/${module}.config ]; then
    echo "// Add predefined macros for your project here. For example:" > ${projectdir}/${module}.config
    echo "// #define YOUR_CONFIGURATION belongs here" >> ${projectdir}/${module}.config
  fi
  # generate a default .creator file if it doesn't exist yet
  if [ ! -f ${projectdir}/${module}.creator ]; then
    echo "[general]" > ${projectdir}/${module}.creator
  fi

  # go back to user directory
  cd $userdir

  # fill the output variables
  if [ ! -z "$outvar" ]; then
    eval $outvar="$projectdir"
  fi
  if [ ! -z "$gccoutvar" ]; then
    eval $gccoutvar="$gccincludedir"
  fi

  return 0
}

### create project files for all modules #######################################
# Create project files for all modules.
#
# usage:      createAllProjects <modules> [-p|--path=<path>] [--gcc=<path>] [-o|--out=<var>] [--gccout=<var>]
# arguments:  <modules>
#                 Array containing all modules available.
#             -p, --path <path>
#                 Path where to create the project files.
#             --gcc=<path>
#                 Path to the GCC include directory.
#             -o, --out <var>
#                 Variable to store the path to.
#             --gccout=<var>
#                 Variable to store the path to the GCC include directory to.
#                 If this optional arguments is absent, ths function will ask for user input.
# return:     0
#                 No error or warning occurred.
#             1
#                 Aborted by user.
#             -1
#                 No modules available.
#
function createAllProjects {
  local modules=("${!1}")
  local projectdir=""
  local gccincludedir=""
  local outvar=""
  local gccoutvar=""

  # parse arguments
  local otherargs=()
  while [ $# -gt 0 ]; do
    if ( parseIsOption $1 ); then
      case "$1" in
        -p=*|--path=*)
          projectdir=$(realpath "${1#*=}"); shift 1;;
        -p|--path)
          projectdir=$(realpath "$2"); shift 2;;
        --gcc=*)
          gccincludedir=$(realpath "${1#*=}"); shift 1;;
        --gcc)
          gccincludedir=$(realpath "$2"); shift 2;;
        -o=*|--out=*)
          outvar=${1#*=}; shift 1;;
        -o|--out)
          outvar=$2; shift 2;;
        --gccout=*)
          gccoutvar=$(realpath "${1#*=}"); shift 1;;
        --gccout)
          gccoutvar=$(realpath "$2"); shift 2;;
        *)
          printError "invalid option: $1\n"; shift 1;;
      esac
    else
      otherargs+=("$1")
      shift 1
    fi
  done

  # sanity check for the modules variable
  if [ -z "${modules[*]}" ]; then
    printError "no modules available\n"
    return -1
  fi

  # read absolute project directory if required
  if [ -z "$projectdir" ]; then
    getProjectDir projectdir
  fi

  # check for existing project files
  local projectfiles=()
  for module in ${modules[@]}; do
    local pfiles="$(find ${projectdir} -maxdepth 1 -type f | grep -E "${module}\.(includes|files|config|creator)$")"
    IFS=$'\n'; pfiles=($pfiles); unset IFS
    projectfiles=( ${projectfiles[*]} ${pfiles[*]} )
  done
  if [ ! -z "${projectfiles[*]}" ]; then
    printWarning "The following files will be removed:\n"
    for pfile in ${projectfiles[@]}; do
      printWarning "\t$(basename $pfile)\n"
    done
    local userinput=""
    printInfo "Continue and overwrite? [y/n]\n"
    readUserInput "YyNn" userinput
    case "${userinput}" in
      Y|y)
        for pfile in ${projectfiles[*]}; do
          rm "$pfile"
        done
        ;;
      N|n)
        printWarning "Project generation aborted by user\n"
        return 1
        ;;
      *)
        printError "unexpected input: ${userinput}\n"
        return 999
        ;;
    esac
  fi

  # print message
  printf "\n"
  printInfo "generating QtCreator project files for all modules...\n"

  # retrieve absolute GCC include path
  if [ -z "$gccincludedir" ]; then
    retrieveGccIncludeDir gccincludedir
  fi

  # iterate over all modules
  local retval=1
  for module in ${modules[@]}; do
    if [ $retval != 0 ]; then
      printf "\n"
    fi
    createModuleProject modules[@] --module="$module" --path="$projectdir" --gcc="$gccincludedir"
    retval=$?
  done

  return 0
}

### delete project files #######################################################
# Deletes all project files and optionally .user files, too.
#
# usage:      deleteProjects [-p|--path=<path>] [-m|--module=<module>] [-o|--out=<var>] [-w|-wipe]
# arguments:  -p, --path <path>
#                 Path where to delete the project files.
#             -m, --module <module>
#                 Module name for which the project files shall be deleted.
#             -o, --out <var>
#                 Variable to store the path to.
#             -w, --wipe
#                 Delete .user files as well.
# return:
#  -  0: no error
#
function deleteProjects {
  local modulename=""
  local projectdir=""
  local outvar=""
  local wipe=false
  local files=""

  # parse arguments
  local otherargs=()
  while [ $# -gt 0 ]; do
    if ( parseIsOption $1 ); then
      case "$1" in
        -p=*|--path=*)
          projectdir=$(realpath "${1#*=}"); shift 1;;
        -p|--path)
          projectdir=$(realpath "$2"); shift 2;;
        -m=*|--module=*)
          modulename="${1#*=}"; shift 1;;
        -m|--module)
          modulename="${2}"; shift 2;;
        -o=*|--out=*)
          outvar=${1#*=}; shift 1;;
        -o|--out)
          outvar=$2; shift 2;;
        -w|--wipe)
          wipe=true; shift 1;;
        *)
          printError "invalid option: $1\n"; shift 1;;
      esac
    else
      otherargs+=("$1")
      shift 1
    fi
  done

  # read absolute project directory if required
  if [ -z "$projectdir" ]; then
    getProjectDir projectdir
  fi

  # list all files to be deleted
  if [ -z "$modulename" ]; then
    if [ $wipe != true ]; then
      files=$(find "${projectdir}" -maxdepth 1 -type f | grep -E "^.+\.(includes|files|config|creator)$")
    else
      files=$(find "${projectdir}" -maxdepth 1 -type f | grep -E "^.+\.(includes|files|config|creator|creator\.user)$")
    fi
  else
    if [ $wipe != true ]; then
      files=$(find "${projectdir}" -maxdepth 1 -type f | grep -E "^.${modulename}\.(includes|files|config|creator)$")
    else
      files=$(find "${projectdir}" -maxdepth 1 -type f | grep -E "^.${modulename}\.(includes|files|config|creator|creator\.user)$")
    fi
  fi
  if [ ! -z "$files" ]; then
    printInfo "Deleting the following files:\n"
    while read line; do
      printInfo "\t$(basename ${line})\n"
      rm ${line} 2>&1 | tee -a $LOG_FILE
    done <<< "${files}"
  else
    printInfo "No project files found\n"
  fi

  # store the path to the output variable, if required
  if [ ! -z "$outvar" ]; then
    eval $outvar="$projectdir"
  fi

  return 0
}

### main function of this script ###############################################
# Creates, deletes and wipes QtCreator project files for the three AMiRo base modules.
#
# usage:      see function printHelp
# arguments:  see function printHelp
# return:     0
#                 No error or warning ocurred.
#
function main {
# print welcome/info text if not suppressed
  if [[ $@ != *"--noinfo"* ]]; then
    printWelcomeText
  else
    printf "######################################################################\n"
  fi
  printf "\n"

  # if --help or -h was specified, print the help text and exit
  if [[ $@ == *"--help"* || $@ == *"-h"* ]]; then
    printHelp
    printf "\n"
    quitScript
  fi

  # set log file if specified
  if [[ $@ == *"--log"* ]] || [[ $@ == *"--LOG"* ]]; then
    # get the parameter (file name)
    local cmdidx=1
    while [[ ! "${!cmdidx}" = "--log"* ]] && [[ ! "${!cmdidx}" = "--LOG"* ]]; do
      cmdidx=$[cmdidx + 1]
    done
    local cmd="${!cmdidx}"
    local logfile=""
    if [[ "$cmd" = "--log="* ]] || [[ "$cmd" = "--LOG="* ]]; then
      logfile=${cmd#*=}
    else
      local filenameidx=$((cmdidx + 1))
      logfile="${!filenameidx}"
    fi
    # optionally force silent appending
    if [[ "$cmd" = "--LOG"* ]]; then
      setLogFile --option=c --quiet "$logfile" LOG_FILE
    else
      setLogFile "$logfile" LOG_FILE
      printf "\n"
    fi
  fi
  # log script name
  printLog "this is $(realpath ${BASH_SOURCE[0]})\n"

  # detect available modules and inform user
  local modules=()
  detectModules modules
  case "${#modules[@]}" in
    0)
      printInfo "no module has been detected\n";;
    1)
      printInfo "1 module has been detected:\n";;
    *)
      printInfo "${#modules[@]} modules have been detected:\n"
  esac
  for (( idx=0; idx<${#modules[@]}; ++idx )); do
    printInfo "  - ${modules[$idx]}\n"
  done
  printf "\n"

  # parse arguments
  local otherargs=()
  while [ $# -gt 0 ]; do
    if ( parseIsOption $1 ); then
      case "$1" in
        -h|--help) # already handled; ignore
          shift 1;;
        -m=*|--module=*)
          createModuleProject modules[@] --module="${1#*=}"; printf "\n"; shift 1;;
        -m*|--module*)
           createModuleProject modules[@] --module="${2}"; printf "\n"; shift 2;;
        -a|--all)
           createAllProjects modules[@]; shift 1;;
        -c|--clean)
          deleteProjects; printf "\n"; shift 1;;
        -w|--wipe)
          deleteProjects --wipe; printf "\n"; shift 1;;
        -q|--quit)
          quitScript; shift 1;;
        --log=*|--LOG=*) # already handled; ignore
          shift 1;;
        --log|--LOG) # already handled; ignore
          shift 2;;
        --noinfo) # already handled; ignore
          shift 1;;
        *)
          printError "invalid option: $1\n"; shift 1;;
      esac
    else
      otherargs+=("$1")
      shift 1
    fi
  done

  # interactive menu
  while ( true ); do
    # main menu info prompt and selection
    printInfo "QtCreator setup main menu\n"
    printf "Please select one of the following actions:\n"
    printf "  [M] - create a project for a single module\n"
    printf "  [A] - create a project for all modules\n"
    printf "  [C] - clean all project files\n"
    printf "  [W] - wipe all project and .user files\n"
    printf "  [Q] - quit this setup\n"
    local userinput=""
    readUserInput "MmAaCcWwQq" userinput
    printf "\n"

    # evaluate user selection
    case "$userinput" in
      M|m)
        createModuleProject modules[@]; printf "\n";;
      A|a)
        createAllProjects modules[@]; printf "\n";;
      C|c)
        deleteProjects; printf "\n";;
      W|w)
        deleteProjects --wipe; printf "\n";;
      Q|q)
        quitScript;;
      *) # sanity check (exit with error)
        printError "unexpected argument: $userinput\n";;
    esac
  done

  exit 0
}

################################################################################
# SCRIPT ENTRY POINT                                                           #
################################################################################

main "$@"

