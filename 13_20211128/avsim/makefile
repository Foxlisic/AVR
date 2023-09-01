all:
	g++ main.cc -I. -O3 glut32.lib -lopengl32 -lglu32 -m32 -o avrsim.exe
	strip avrsim.exe
	avrsim.exe ../test/main.bin