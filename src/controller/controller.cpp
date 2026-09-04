#include "controller.h"

#include "../model/world.h"
#include "../view/View.h"
#include <iostream>
#include <fstream>

Controller::Controller(World &world, View &view)
    : world(world), view(view)
{
    record = loadRecord(); // Carica il record all'inizio del gioco
}

void Controller::runFrame(float deltaTime, FrameInput &input)
{
    // ====== 1. RENDER - Passa il punteggio giusto in base allo stato ======
    int displayPoints = (state == GameState::Playing) ? world.getPoints() : record;
    const UiCommand uiCommand = view.render(state, displayPoints);

    // ====== 2. GESTIONE UI COMMANDS (hanno priorità) ======
    if (uiCommand == UiCommand::StartGame)
    {
        state = GameState::Playing;
        input.shootJustPressed = false;
        view.clearRenderQueue();
        view.clearTextQueue();
        view.buildRenderQueue(world);
        return;
    }

    if (uiCommand == UiCommand::Exit)
    {
        state = GameState::StartMenu;
        record = loadRecord(); // Ricarica il record quando torna al menù
        view.clearRenderQueue();
        world.worldClear();
        return;
    }

    if (uiCommand == UiCommand::BackToMenu)
    {
        state = GameState::StartMenu;
        record = loadRecord(); // Ricarica il record quando torna al menù
        view.clearRenderQueue();
        view.clearTextQueue();
        world.worldClear();
        return;
    }

    // ====== 3. UPDATE MONDO SOLO SE PLAYING ======
    if (state == GameState::Playing)
    {
        // Movimento player
        world.movePlayer(input.moveAxis, deltaTime);

        if (input.shootJustPressed)
        {
            glm::vec2 pPos = world.getPlayer().getPosition() + glm::vec2(0.0f, 1.0f);
            glm::vec2 direction = glm::normalize(input.mouseWorldPos - pPos);
            world.fireBalls(direction);
        }

        // Update mondo
        bool gameRunning = world.update(deltaTime);
        if (!gameRunning)
        {
            if (world.getPoints() > record)
            {
                record = world.getPoints();
                writeRecord();
            }
            view.clearRenderQueue();
            view.clearTextQueue();
            world.worldClear();
            state = GameState::GameOver;
            return;
        }

        // Build render queue per il frame corrente
        view.buildRenderQueue(world);
    }
}

void Controller::showStartMenu()
{
    state = GameState::StartMenu;
    record = loadRecord(); // Carica il record quando mostri il menù
}

GameState Controller::getState() const
{
    return state;
}

void Controller::writeRecord()
{
    std::ofstream outFile(ASSETS_DIR "/record.txt");
    if (outFile.is_open())
    {
        outFile << world.getPoints() << std::endl;
        outFile.close();
    }
    else
    {
        std::cerr << "Unable to open record file for writing." << std::endl;
    }
}

int Controller::loadRecord()
{
    std::ifstream inFile(ASSETS_DIR "/record.txt");
    int record = 0;
    if (inFile.is_open())
    {
        inFile >> record;
        inFile.close();
    }
    else
    {
        std::cerr << "Unable to open record file for reading." << std::endl;
    }
    return record;
}