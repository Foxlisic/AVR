class Video
{
protected:

    uint8_t bank;

public:

    void flip() { out(1, bank); bank = (1 - bank) & 1; out(2, bank); }

    void pset(uint16_t x, uint16_t y, uint8_t cl) {

        if (x >= 320 || y >= 200) return;

        uint16_t A = y*320 + x;

        out(0, A >> 8);
        out(0, A);
        out(3, cl);
    }

};
