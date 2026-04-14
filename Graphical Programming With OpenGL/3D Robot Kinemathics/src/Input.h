#ifndef INPUT_H
#define INPUT_H

class App;

namespace Input
{
    void bind(App* app);
    void registerCallbacks();
    void printControls();

    void keyboard(unsigned char key, int x, int y);
    void keyboardUp(unsigned char key, int x, int y);
    void mouseButton(int button, int state, int x, int y);
    void mouseMove(int x, int y);
}

#endif