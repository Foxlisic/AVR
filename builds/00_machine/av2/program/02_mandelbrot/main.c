#include <avrio.c>
#include <pset.c>

// z(i+1) = z(i)^2 + C[r,i]
int bailout(float r, float i) {
    
    int k;
    float zi = 0.0, zr = 0.0;    
    for (k = 0; k < 32; k++) {
        
        float r_ = zr*zr - zi*zi + r;
        float i_ = 2*zi*zr + i;
        
        zr = r_; zi = i_;    
        
        if (zr*zr + zi*zi > 4.0) {
            return k;
        }                
    }
    
    return 0;
}

void mandelbrot() {

    for (int y = -120; y < 120; y++) {
    for (int x = -128; x < 128; x++) {

        float r = ((float)x / 64.0),
              i = ((float)y / 64.0);

        int b = bailout(r, i);
        pset(128 + x, 120 + y, b);
    } }
}

int main() {

    outp(0x01, 2);
    mandelbrot();
    for(;;);
}
