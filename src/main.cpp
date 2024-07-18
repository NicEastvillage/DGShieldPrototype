#include <iostream>
#include "raylibns.h"
#include "Model.h"
#include "ShieldGenerator.h"
#include "Simulator.h"

using Model = DGShield::Model;
using ShieldGenerator = DGShield::ShieldGenerator;
using Simulator = DGShield::Simulator;

int main() {
    std::cout << "Hello, World!" << std::endl;

//    Model m(12, 8);
//    m.addDanger({{3, 5}, {7, 8}});
//    m.addDanger({{10, 0}, {10, 4}});

//    Model m(36, 24);
//    m.addDanger({{8, 15}, {21, 24}});
//    m.addDanger({{28, 0}, {30, 12}});
//    m.addDanger({{11, 9}, {13, 12}});
//    m.addDanger({{19, 13}, {20, 15}});
//    m.addDanger({{6, 0}, {36, 3}});

    Model m(150, 100);
    m.addDanger({{18,  22}, {150, 0}});
    m.addDanger({{52, 40}, {150, 0}});
    m.addDanger({{100, 75}, {115, 42}});
    m.addDanger({{48, 70}, {70, 98}});
    m.addDanger({{125, 90}, {150, 100}});

//    Model m(600, 400);
//    m.addDanger({{70,  90}, {600, 0}});
//    m.addDanger({{210, 160}, {600, 0}});
//    m.addDanger({{400, 300}, {460, 170}});
//    m.addDanger({{190, 280}, {280, 390}});
//    m.addDanger({{500, 375}, {600, 400}});

    Simulator sim(m);
    ShieldGenerator gen(m);

    rl::InitWindow(m.width * rl::TILE_SIZE, m.height * rl::TILE_SIZE, "Dependency Graph Shield Refinement");

    while (!rl::WindowShouldClose())
    {
        if (rl::IsKeyPressed(rl::KEY_RIGHT)) sim.step(DGShield::action_t::FORWARD);
        if (rl::IsKeyPressed(rl::KEY_UP)) sim.step(DGShield::action_t::UP);
        if (rl::IsKeyPressed(rl::KEY_DOWN)) sim.step(DGShield::action_t::DOWN);
        if (rl::IsKeyPressed(rl::KEY_P)) sim.finishWithRandomMoves();
        if (rl::IsKeyPressed(rl::KEY_O)) sim.restart();
        if (rl::IsKeyDown(rl::KEY_L)) sim.step(DGShield::action_t(rand() % 3));

        if (rl::IsKeyPressed(rl::KEY_Q)) gen.reset();
        if (rl::IsKeyPressed(rl::KEY_W)) gen.step();
        if (rl::IsKeyPressed(rl::KEY_E)) gen.runToCompletion();

        rl::BeginDrawing();
        ClearBackground(rl::RAYWHITE);
        m.render();
        sim.render();
        gen.render();
        rl::EndDrawing();
    }

    rl::CloseWindow();

    return 0;
}
