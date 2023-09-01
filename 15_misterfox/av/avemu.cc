#include "av.h"

int main(int argc, char* argv[]) {

    AVR* app = new AVR(640, 400, 1, 25);

    app->load(argc, argv);

    // Обработка возникшего события
    while (app->main()) {
        app->frame();
    }

    return 0;
}
