if scons --optimize=Dbg --target=Tests --jobs=8 ; then
    # We try testing in a topological order
	./NetworKit-Tests-Dbg --gtest_filter="Trade*":"IM*":"GlobalTrade*":"UniformTrade*":"Curveball*":"EdgeSwitchingMarkovChainRandomization*"
else
    echo "Build unsuccessful." 
fi
