#include <app.h>

App* app;

int main(int argc, char* argv[]) {

    app = new App(768, 720, "AVRCore BK0010 Screen");

    app->init(argc, argv);
    app->update();

    // опциональненько
    //app->start(); 

    return 0;
}
