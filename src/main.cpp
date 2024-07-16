#include <iostream>
#include "raylibns.h"
#include "Model.h"
#include "ShieldGenerator.h"
#include "Simulator.h"

const int WIDTH = 600;
const int HEIGHT = 400;

using Model = DGShield::Model;
using ShieldGenerator = DGShield::ShieldGenerator;
using Simulator = DGShield::Simulator;

int main() {
    std::cout << "Hello, World!" << std::endl;

    rl::InitWindow(WIDTH * rl::TILE_SIZE, HEIGHT * rl::TILE_SIZE, "Dependency Graph Shield Refinement");

    Model m(WIDTH, HEIGHT);
    m.makeDangerous({{70, 90}, {600, 0}});
    m.makeDangerous({{210, 160}, {600, 0}});
    m.makeDangerous({{400, 300}, {460, 170}});
    m.makeDangerous({{190, 280}, {280, 390}});
    m.makeDangerous({{500, 375}, {600, 400}});

    Simulator sim(m);

    ShieldGenerator gen(m);

    while (!rl::WindowShouldClose())
    {
        if (rl::IsKeyPressed(rl::KEY_RIGHT)) sim.step(DGShield::action_t::FORWARD);
        if (rl::IsKeyPressed(rl::KEY_UP)) sim.step(DGShield::action_t::UP);
        if (rl::IsKeyPressed(rl::KEY_DOWN)) sim.step(DGShield::action_t::DOWN);
        if (rl::IsKeyPressed(rl::KEY_P)) sim.finishWithRandomMoves();
        if (rl::IsKeyPressed(rl::KEY_O)) sim.restart();
        if (rl::IsKeyDown(rl::KEY_L)) sim.step(DGShield::action_t(rand() % 3));

        if (rl::IsKeyPressed(rl::KEY_N) && !gen.isDone()) gen.step();

        rl::BeginDrawing();
        ClearBackground(rl::RAYWHITE);
        m.render();
        gen.render();
        sim.render();
        rl::EndDrawing();
    }

    rl::CloseWindow();

    return 0;
}
