#include "controller/controller.h"
#include "controller/input.h"
#include "model/world.h"
#include "view/View.h"
#include <GLFW/glfw3.h>
#include <algorithm>

int main()
{
    World world;
    View view;
    Controller controller(world, view);
    InputManager inputManager(nullptr);

    bool playing = true;

    constexpr float MAX_DT = 1.0f / 30.0f; // evita salti di simulazione dopo un frame lento (es. resize/drag della finestra)

    view.initView();
    inputManager.setWindow(view.getWindow());

    double lastTime = glfwGetTime();

    while (!view.shouldClose())
    {
        double currentTime = glfwGetTime();
        float dt = std::min(static_cast<float>(currentTime - lastTime), MAX_DT);
        lastTime = currentTime;

        view.pollEvents();
        inputManager.setViewportInfo(view.getViewPortInfo());
        FrameInput input = inputManager.poll();
        controller.runFrame(dt, input);
    }

    view.shutdownView();

    return 0;
}
