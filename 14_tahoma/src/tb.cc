#include "app.cc"

int main(int argc, char** argv) {

    int   instr   = 125000;
    int   maximum = 0;
    float target  = 100;

    Verilated::commandArgs(argc, argv);
    App* app = new App(argc, argv);

    while (app->main()) {

        Uint32 start = SDL_GetTicks();

        // Есть ограничение по скорости
        if (maximum && maximum < instr) instr = maximum;

        // Автоматическая коррекция кол-ва инструкции в секунду
        for (int i = 0; i < instr; i++) app->tick();

        // Коррекция тактов
        Uint32 delay = (SDL_GetTicks() - start);

        instr = (instr * (0.5 * target) / (float)delay);
        instr = instr < 1000 ? 1000 : instr;
    }

    return app->destroy();
}
