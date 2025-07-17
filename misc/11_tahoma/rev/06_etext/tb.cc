#include "tb.h"

int main(int argc, char* argv[]) {

    App* app = new App(argc, argv);
    while (app->main()) app->frame();
    return app->destroy();
}
