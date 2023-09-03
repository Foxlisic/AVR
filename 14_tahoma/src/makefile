VLIB=/usr/share/verilator/include

#all: icarus
all: icarus tbc
icarus:
	iverilog -g2005-sv -DICARUS=1 -o tb.qqq tb.v core.v
	vvp tb.qqq >> /dev/null
	rm tb.qqq
vcd:
	gtkwave tb.vcd
wave:
	gtkwave tb.gtkw
tbc: verilated
	g++ -o tb -I$(VLIB) $(VLIB)/verilated.cpp tb.cc -lSDL2 \
	obj_dir/Vvga__ALL.a \
	obj_dir/Vps2__ALL.a \
	obj_dir/Vcore__ALL.a

verilated:
	verilator -cc vga.v
	verilator -cc ps2.v
	verilator -cc core.v
	cd obj_dir && make -f Vvga.mk
	cd obj_dir && make -f Vps2.mk
	cd obj_dir && make -f Vcore.mk
clean:
	rm -rf db incremental_db simulation timing greybox_tmp *.jdi *.pof *.sld *.rpt *.summary *.sof *.done *.pin *.qws *.bak *.smsg *.qws *.vcd *.qqq *.jic *.map .qsys_edit undo_redo.txt PLLJ_PLLSPE_INFO.txt c5_pin_model_dump.txt tb obj_dir
