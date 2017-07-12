if scons --optimize=Dbg --target=Tests ; then
    ./NetworKit-Tests-Dbg --gtest_filter="IM*":"*Trade*"
else
    echo "Build unsuccessful." 
fi
