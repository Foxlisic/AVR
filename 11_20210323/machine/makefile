all:
	iverilog -g2005-sv -DICARUS=1 -o tb.qqq tb.v avrcpu.v 
	vvp tb.qqq >> /dev/null
vcd:
	gtkwave tb.vcd
wave:
	gtkwave tb.gtkw
clean:
	rm -rf *.qqq *.vcd
