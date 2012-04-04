#!/bin/bash
# bamboo.sh

# Description:

# A shell script to command a build from the Atlassian Bamboo
# Continuous Integration Environment.

# Because there are pre-make steps that need to occur due to the use
# of the GNU Autotools, this script simplifies the build activation by
# consolidating the build steps.

# Assume that fresh code revision has been downloaded by Bamboo from
# the SST Google Code repository prior to invocation of this
# script. Plow through the build, exiting if something goes wrong.

#=========================================================================
# Definitions
#=========================================================================
# Root of directory checked out, where this script should be found
export SST_ROOT=`pwd`

# Location of SST library dependencies (deprecated)
export SST_DEPS=${HOME}/local
# Location where SST files are installed
export SST_INSTALL=${HOME}/local
# Location where SST build files are installed
export SST_INSTALL_BIN=${SST_INSTALL}/bin

# Location where SST dependencies are installed. This only specifies
# the root; dependencies may be installed in various locations under
# this directory. The user can override this value by setting the
# exporting the SST_INSTALL_DEPS_USER variable in their environment.
export SST_INSTALL_DEPS=${HOME}/local
# Initialize build type to null
export SST_BUILD_TYPE=""
# Load test definitions
. test/include/testDefinitions.sh
# Load dependency definitions
. deps/include/depsDefinitions.sh

# Uncomment the following line or export from your environment to
# retain binaries after build
#export SST_RETAIN_BIN=1
#=========================================================================
#Functions
#=========================================================================

#-------------------------------------------------------------------------
# Function: dotests
# Description:
#   Purpose:
#       Based on build type and architecture, run tests
#   Input:
#       $1 (build type): kind of build to run tests for
#   Output: none
#   Return value: 0 if success
dotests() {
    # Build type is available as SST_BUILD_TYPE global, if
    # needed to be selective about the tests that are run.

    # NOTE: Bamboo does a fresh checkout of code each time, so there
    # are no residuals left over from the last build. The directories
    # initialized here are ephemeral, and not kept in CM/SVN.

    # Initialize directory to hold testOutputs
    rm -Rf ${SST_TEST_OUTPUTS}
    mkdir -p ${SST_TEST_OUTPUTS}

    # Initialize directory to hold Bamboo-compatible XML test results
    rm -Rf ${SST_TEST_RESULTS}
    mkdir -p ${SST_TEST_RESULTS}

    # Initialize directory to hold temporary test input files
    rm -Rf ${SST_TEST_INPUTS_TEMP}
    mkdir -p ${SST_TEST_INPUTS_TEMP}

    # Run test suites

    # DO NOT pass args to the test suite, it confuses
    # shunit. Use an environment variable instead.
    ${SST_TEST_SUITES}/testSuite_portals.sh
    # Add other test suites here, i.e.
    # ${SST_TEST_SUITES}/testSuite_moe.sh
    # ${SST_TEST_SUITES}/testSuite_larry.sh
    # ${SST_TEST_SUITES}/testSuite_curly.sh
    # ${SST_TEST_SUITES}/testSuite_shemp.sh
    # etc.

    # Purge SST installation 
    if [[ ${SST_RETAIN_BIN:+isSet} != isSet ]]
    then
        rm -Rf ${SST_INSTALL}
    fi

}


#-------------------------------------------------------------------------
# Function: getconfig
# Description:
#   Purpose:
#       Based on build type and architecture, generate 'configure'
#       parameters.
#   Input:
#       $1 (build type): kind of build to configure for
#       $2 (architecture): build platform architecture from uname
#   Output: string containing 'configure' parameters
#   Return value: none
getconfig() {

    # Configure default dependencies to use if nothing is explicitly specified
    local defaultDeps="-k default -d default -p default -z default -b default -g default -m default -i default -o default -h default -s none"

    local depsStr=""

    # These base options get applied to every 'configure'
    local baseoptions="--disable-silent-rules --prefix=$SST_INSTALL --with-boost=$SST_DEPS_INSTALL_BOOST --with-zoltan=$SST_DEPS_INSTALL_ZOLTAN --with-parmetis=$SST_DEPS_INSTALL_PARMETIS"

    local cc_compiler=`which mpicc`
    local cxx_compiler=`which mpicxx`

    # make sure that sstmacro is suppressed
    if [ -e ./sst/elements/macro_component/.unignore ] && [ -f ./sst/elements/macro_component/.unignore ]
    then
        rm ./sst/elements/macro_component/.unignore
    fi

    case $1 in
        Disksim_test)
            #-----------------------------------------------------------------
            # Disksim_test
            #     This option used for configuring SST for use with DiskSim
            #-----------------------------------------------------------------
            # TAU installs in architecture-specific directories
            case $2 in
                x86_64)
                    # 64-bit Linux 
                    disksimdir="x86_64"
                    ;;
                i686)
                    # 32-bit Linux 
                    disksimdir="i386_linux"
                    ;;
            esac

            # Environment variables used for Disksim config
            disksimenv="CFLAGS=-DDISKSIM_DBG CFLAGS=-g CXXFLAGS=-g"

            configStr="$baseoptions --with-boost-mpi --with-dramsim=no --with-disksim=$SST_INSTALL_DEPS/$disksimdir --no-recursion $disksimenv"
            depsStr="$defaultDeps"
            ;;
        PowerTherm_test)
            #-----------------------------------------------------------------
            # PowerTherm_test
            #     This option used for configuring SST with Power and
            #     Therm enabled
            #-----------------------------------------------------------------
            configStr="$baseoptions --with-McPAT=$SST_DEPS_INSTALL_MCPAT --with-hotspot=$SST_DEPS_INSTALL_HOTSPOT --with-orion=$SST_DEPS_INSTALL_ORION"
            depsStr="$defaultDeps"
            ;;
        dramsim_test)
            #-----------------------------------------------------------------
            # dramsim_test
            #     This option used for configuring SST with DRAMSim enabled
            #-----------------------------------------------------------------
            configStr="$baseoptions --with-dramsim=$SST_DEPS_INSTALL_DRAMSIM"
            depsStr="$defaultDeps"
            ;;
        gem5_test)
            #-----------------------------------------------------------------
            # gem5_test
            #     This option used for configuring SST with gem5 enabled
            #-----------------------------------------------------------------
            gem5env="CC=${cc_compiler} CXX=${cxx_compiler} CFLAGS=-I/usr/include/python2.6 CXXFLAGS=-I/usr/include/python2.6"
            configStr="$baseoptions --with-gem5=$SST_DEPS_INSTALL_GEM5SST --with-gem5-build=opt $gem5env"
            depsStr="$defaultDeps"
            ;;
        sstmacro_latest_test)
            #-----------------------------------------------------------------
            # sstmacro_latest_test
            #     This option used for configuring SST with latest devel sstmacro
            #-----------------------------------------------------------------
            echo "$USER" > ./sst/elements/macro_component/.unignore
            gem5env="CC=${cc_compiler} CXX=${cxx_compiler} CFLAGS=-I/usr/include/python2.6 CXXFLAGS=-I/usr/include/python2.6"
            configStr="$baseoptions --with-gem5=$SST_DEPS_INSTALL_GEM5SST --with-gem5-build=opt $gem5env"
            depsStr="-k default -d default -p default -z default -b default -g default -m default -i default -o default -h default -s stabledevel"
            ;;
        sstmacro_2.2.0_test)
            #-----------------------------------------------------------------
            # sstmacro_2.2.0_test
            #     This option used for configuring SST with sstmacro 2.2.0
            #-----------------------------------------------------------------
            echo "$USER" > ./sst/elements/macro_component/.unignore
            gem5env="CC=${cc_compiler} CXX=${cxx_compiler} CFLAGS=-I/usr/include/python2.6 CXXFLAGS=-I/usr/include/python2.6"
            configStr="$baseoptions --with-gem5=$SST_DEPS_INSTALL_GEM5SST --with-gem5-build=opt $gem5env"
            depsStr="-k default -d default -p default -z default -b default -g default -m default -i default -o default -h default -s 2.2.0"
            ;;
        dramsim_latest_test)
            #-----------------------------------------------------------------
            # dramsim_test
            #     This option used for configuring SST with latest devel DRAMSim 
            #-----------------------------------------------------------------
            configStr="$baseoptions --with-dramsim=$SST_DEPS_INSTALL_DRAMSIM"
            depsStr="-k default -d stabledevel -p default -z default -b default -g default -m default -i default -o default -h default -s none"
            ;;
        boost_1.49_test)
            #-----------------------------------------------------------------
            # dramsim_test
            #     This option used for configuring SST with latest devel DRAMSim 
            #-----------------------------------------------------------------
            gem5env="CC=${cc_compiler} CXX=${cxx_compiler} CFLAGS=-I/usr/include/python2.6 CXXFLAGS=-I/usr/include/python2.6"
            configStr="$baseoptions --with-gem5=$SST_DEPS_INSTALL_GEM5SST --with-gem5-build=opt $gem5env"
            depsStr="-k default -d default -p default -z default -b 1.49 -g default -m default -i default -o default -h default -s none"
            ;;
        default|*)
            configStr="$baseoptions --with-dramsim=$SST_DEPS_INSTALL_DRAMSIM"
            depsStr="$defaultDeps"
            ;;
    esac

    export SST_SELECTED_DEPS="$depsStr"
    export SST_SELECTED_CONFIG="$configStr"
#    echo $configStr
}


#-------------------------------------------------------------------------
# Function: dobuild
# Description:
#   Purpose: Performs the actual build
#   Input:
#     -t <build type>
#     -a <architecture>
#   Output: none
#   Return value: 0 if success
dobuild() {

    # process cmdline options
    OPTIND=1
    while getopts :t:a: opt
    do
        case "$opt" in
            t) # build type
                local buildtype=$OPTARG
                ;;
            a) # architecture
                local architecture=$OPTARG
                ;;
            *) # unknown option 
                echo "dobuild () : Unknown option $opt"
                return 126 # command can't execute
                ;;
        esac
    done

    export PATH=$SST_INSTALL_BIN:$PATH

    # obtain dependency and configure args
    getconfig $buildtype $architecture

    # after getconfig is run,
    # $SST_SELECTED_DEPS now contains selected dependencies 
    # $SST_SELECTED_CONFIG now contains config line

    # based on buildtype, configure and build dependencies
    # build, patch, and install dependencies
    $SST_DEPS_BIN/sstDependencies.sh $SST_SELECTED_DEPS cleanBuild
    retval=$?
    if [ $retval -ne 0 ]
    then
        return $retval
    fi

    # load convenience variables
    $SST_DEPS_BIN/sstDependencies.sh $SST_SELECTED_DEPS queryEnv > $HOME/SST_deps_env.sh
    source $HOME/SST_deps_env.sh

    export PYTHON_DEV_INCLUDE=/usr/include/python2.6
    export LD_LIBRARY_PATH=${SST_INSTALL_DEPS}/lib:${SST_INSTALL_DEPS}/lib/sst:${PYTHON_DEV_INCLUDE}:${LD_LIBRARY_PATH}
    # autogen to create ./configure
    ./autogen.sh
    retval=$?
    if [ $retval -ne 0 ]
    then
        return $retval
    fi

    echo "bamboo.sh: config args = $SST_SELECTED_CONFIG"
    ./configure $SST_SELECTED_CONFIG
    retval=$?
    if [ $retval -ne 0 ]
    then
        return $retval
    fi

    # build SST
    make all
    retval=$?
    if [ $retval -ne 0 ]
    then
        return $retval
    fi

    # print build and linkage information for warm fuzzy
    echo "SSTBUILD============================================================"
    echo "Built SST with configure string"
    echo "    ./configure ${SST_SELECTED_CONFIG}"
    echo "----------------"
    echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}"
    echo "----------------"
    echo "sst exectuable linkage information"
    echo "$ ldd ./sst/core/sst.x"
    ldd ./sst/core/sst.x
    echo "SSTBUILD============================================================"

    # install SST
    make install
    retval=$?
    if [ $retval -ne 0 ]
    then
        return $retval
    fi

}

#=========================================================================
# main

retval=0

if [ $# -ne 2 ]
then
    # need build type and MPI type as argument
    #   MPI type = ompi_stable, ompi_[version],
    #              mpich2_stable, mpich_[version]
    echo "Usage : $0 <buildtype> <mpitype>"
    exit 0

else

    # Determine architecture
    arch=`uname -p`
    # Determine kernel name (Linux or MacOS i.e. Darwin)
    kernel=`uname -s`

    case $1 in
        default|PowerTherm_test|Disksim_test|sstmacro_latest_test|sstmacro_2.2.0_test|dramsim_latest_test|dramsim_test|boost_1.49_test|gem5_test)
            # Configure MPI (Linux only)
            if [ $kernel != "Darwin" ] && [ "$MODULESHOME" ]
            then
                # For some reason, .bashrc is not being run prior to
                # this script. Kludge initialization of modules.
                . $HOME/modulesSingleUser/Modules/default/init/bash

                # Load modules in $HOME/privatemodules
                module load use.own
                case $2 in
                    mpich2_stable)
                        echo "MPICH2 stable (mpich2-1.4.1p1) selected"
                        module load mpich2/mpich2-1.4.1p1;;
                    *)
                        echo "OpenMPI stable (openmpi-1.4.4, default) selected"
                        module load openmpi/openmpi-1.4.4;;
                esac

            fi

            # Build type given as argument to this script
            export SST_BUILD_TYPE=$1

            dobuild -t $SST_BUILD_TYPE -a $arch
            retval=$?
            ;;

        *)
            echo "$0 : unknown action \"$1\""
            retval=1
            ;;
    esac
fi

if [ $retval -eq 0 ]
then
    # Build was successful, so run tests, providing command line args
    # as a convenience. SST binaries must be generated before testing.
    dotests $1
fi

if [ $retval -eq 0 ]
then
    echo "$0 : exit success."
else
    echo "$0 : exit failure."
fi

exit $retval
