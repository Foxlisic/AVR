// Деление двух чисел
vec2i div16(uint a, uint b) {
	
	uint i;
	uint c = 0, q = 0, r = 0;
		
	for (i = 0; i < 16; i++) {

		q = (q<<1) | (a>>15);
		a <<= 1;		
		r <<= 1;
		if (q <= b) {
			r |= 1;
			q -= b;
		}
	}
	
	return {r, q};
}