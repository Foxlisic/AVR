CONF=-lSDL2
VPAR=-Wall -Wno-unused -Wno-width
VINC=/usr/share/verilator/include

all: vga
	g++ -o avr -I$(VINC) avr.cc $(VINC)/verilated.cpp obj_dir/Vga__ALL.a obj_dir/Vavr__ALL.a obj_dir/Vkb__ALL.a $(CONF)
	strip avr
	./avr
vga:
	verilator $(VPAR) -cc ga.v
	verilator $(VPAR) -cc kb.v
	verilator $(VPAR) -cc avr.v
	cd obj_dir && make -f Vga.mk
	cd obj_dir && make -f Vavr.mk
	cd obj_dir && make -f Vkb.mk
video:
	ffmpeg -framerate 60 -r 60 -i temp/record.ppm -vf "scale=w=1280:h=800,pad=width=1920:height=1080:x=320:y=140:color=black" -sws_flags neighbor -sws_dither none -f mp4 -q:v 0 -vcodec mpeg4 -y record.mp4
clean:
	rm -rf avr temp/*.ppm *.o obj_dir
