#!/bin/bash
INST_DIR=$(pwd)

echo -e "\e[31mRebuild networkit for python3-venv\e[39m"
. python-env/bin/activate
cd curveball-im
python3 setup.py build_ext --inplace --optimize=Opt --jobs=20
python3 setup.py clean
