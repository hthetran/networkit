#!/bin/bash
PYTHOND=$(type -p python2)
PYTHON=$(type -p python3)
export CXX=$(type -p g++)
trap "exit" INT

# check gcc version
currentver="$(gcc -dumpversion)"
requiredver="5.0.0"
if [ "$(printf "$requiredver\n$currentver" | sort -V | head -n1)" == "$currentver" ] && [ "$currentver" != "$requiredver" ]; then
	echo "GCC version too old, execute 'module initadd gcc/5.2.0' and log-out"
	exit
fi

currentver="$(python3 --version | perl -pe 's/[^\d\.]//g')"
requiredver="3.4.0"
if [ "$(printf "$requiredver\n$currentver" | sort -V | head -n1)" == "$currentver" ] && [ "$currentver" != "$requiredver" ]; then
	echo "PYTHON version too old, execute 'module initadd python/3.4.3' and log-out"
	exit
fi

echo "Will use the following environment"
echo " python: $PYTHON"
echo " g++:    $CXX"

echo "Will build within current directory; Press CTRL+C to abort"

INST_DIR=$(pwd)

echo -e "\e[31mCheck out repos and build python dependencies \e[39m"

cd $INST_DIR
echo -e "\e[31mCheck out curveball-im \e[39m"
git clone git@git.ae.cs.uni-frankfurt.de:curveball-im
cd curveball-im
git submodule init
git submodule update
cd googletest/googletest
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=ON ..
make -j

cd $INST_DIR
echo -e "\e[31mBuild python3 dependencies \e[39m"
$PYTHON -m venv $INST_DIR/python-env
export NPY_NUM_BUILD_JOBS=20
. $INST_DIR/python-env/bin/activate

type pip3
pip3 install --upgrade pip

cd $INST_DIR/curveball-im
perl -pi -e 's/\{\}/{0}/' SConstruct
cp build.conf.example build.conf
perl -pi -e 's/^cpp\s*=.*$/cpp = $ENV{CXX}/' build.conf
perl -pi -e 's/^std\s*=.*$/std = stx-btree\/include\//' build.conf
perl -pi -e 's/scons_available = True/scons_available = False/' setup.py
sed -i '7s/.*/gtest = googletest\/googletest\/include\//' build.conf
sed -i '10s/.*/gtest = googletest\/googletest\/build\//' build.conf

python3 setup.py build_ext --inplace --optimize=Opt -j20
pip install -e ./
pip install -r requirements.txt
python3 setup.py clean --optimize=Opt

MPL_RC=$(python3 -c 'import matplotlib as mpl; print(mpl.matplotlib_fname())')
perl -pi -e 's/backend\s*:\s*TkAgg/backend: pdf/' $MPL_RC

cd $INST_DIR
echo -e "\e[31mBuild python2 for scons \e[39m"
pip install virtualenv
virtualenv -p $PYTHOND $INST_DIR/python2-env
deactivate
. $INST_DIR/python2-env/bin/activate
pip install scons
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$INST_DIR/curveball-im/googletest/googletest/build/
cd curveball-im
scons --optimize=Dbg --target=Tests --jobs=20
