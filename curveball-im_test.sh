export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:googletest/googletest/build/
if scons --optimize=Dbg --target=Tests --jobs=8 ; then
    # We try testing in a topological order
	./NetworKit-Tests-Dbg --gtest_filter="Trade*":"IM*":"GlobalTrade*":"UniformTrade*":"Curveball*":"EdgeSwitchingMarkovChainRandomization*":"AutocorrelationAnalysis*"
else
    echo "Build unsuccessful." 
fi
