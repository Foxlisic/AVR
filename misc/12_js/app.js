
class App extends Dos2D {

    init() {

        // Запуск ядра AVR
        this.core = new Core({
            program: program,
            put:     this.put.bind(this),
            get:     this.get.bind(this)
        });

        this.cls();
        this.font(16);
    }

    // Реализация ввода-вывода
    put(a, d) {

        if (a >= 0xF000) {

            let x = (a - 0xF000) % 80;
            let y = Math.trunc((a - 0xF000) / 80);
            this.print_font(8*x, 16*y, d);
        }
    }

    // Ничего не делается
    get(a) { return -1; }

    // 50 кадров в секунду x 75000 ~ 3.75 mhz
    loop() {

        let cycles = 0;
        let a = (new Date()).getTime();
        do { cycles += this.core.step(); } while (cycles <= 75000);
        let b = (new Date()).getTime();
        // console.log(b-a, cycles); // Допустимо до 20 мс
    }
}
