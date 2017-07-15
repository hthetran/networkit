if scons --optimize=Dbg --target=Tests ; then
    # We try testing in a topological order
	./NetworKit-Tests-Dbg --gtest_filter="Trade*":"IM*":"GlobalTrade*":"UniformTrade*":"Curveball*"
else
    echo "Build unsuccessful." 
fi
