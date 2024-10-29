all: kpl
	./tb
kpl: vrl
	g++ -o tb \
		-I/usr/share/verilator/include /usr/share/verilator/include/verilated.cpp \
		tb.cc \
		obj_dir/Vgpu__ALL.a \
		obj_dir/Vcpu__ALL.a \
		obj_dir/Vps2__ALL.a \
		-lSDL2_mixer -lSDL2
	strip tb
vrl:
	verilator -cc gpu.v
	verilator -cc cpu.v
	verilator -cc ps2.v
	cd obj_dir && make -f Vgpu.mk
	cd obj_dir && make -f Vcpu.mk
	cd obj_dir && make -f Vps2.mk
clean:
	rm -rf tb obj_dir
