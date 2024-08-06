#include <iostream>
#include "raylibns.h"
#include "Model.h"
#include "ShieldGeneratorDG.h"
#include "Simulator.h"

using Model = DGShield::Model;
using ShieldGeneratorDG = DGShield::ShieldGeneratorDG;
using shield_render_mode_t = DGShield::shield_render_mode_t;
using Simulator = DGShield::Simulator;
using ivec = DGShield::ivec;
using irect = DGShield::irect;

int main() {
//    Model m(12, 8);
//    m.addDanger({{3, 5}, {7, 8}});
//    m.addDanger({{11, 0}, {11, 4}});

//    Model m(36, 24);
//    m.addDanger({{8, 15}, {21, 24}});
//    m.addDanger({{28, 0}, {30, 12}});
//    m.addDanger({{11, 9}, {13, 12}});
//    m.addDanger({{19, 13}, {20, 15}});
//    m.addDanger({{6, 0}, {36, 3}});

//    Model m(150, 100);
//    m.addDanger({{18,  22}, {150, 0}});
//    m.addDanger({{52, 40}, {150, 0}});
//    m.addDanger({{100, 75}, {115, 42}});
//    m.addDanger({{48, 70}, {70, 98}});
//    m.addDanger({{125, 90}, {150, 100}});

    Model m(300, 200);
    m.addDanger({{35,  45}, {300, 0}});
    m.addDanger({{105, 80}, {300, 0}});
    m.addDanger({{200, 150}, {230, 82}});
    m.addDanger({{95, 140}, {140, 195}});
    m.addDanger({{250, 180}, {300, 200}});

//    Model m(600, 400);
//    m.addDanger({{70,  90}, {600, 0}});
//    m.addDanger({{210, 160}, {600, 0}});
//    m.addDanger({{400, 300}, {460, 170}});
//    m.addDanger({{190, 280}, {280, 390}});
//    m.addDanger({{500, 375}, {600, 400}});

    ShieldGeneratorDG gen(m);
    Simulator sim(m, gen);

    rl::InitWindow(m.width * rl::TILE_SIZE, m.height * rl::TILE_SIZE, "Dependency Graph Shield Refinement");

    shield_render_mode_t shieldRenderMode = DGShield::RMODE_LOCAL_SAFETY;
    bool addingDanger = false;
    ivec addingDangerStart(0, 0);
    ivec addingDangerEnd(0, 0);

    while (!rl::WindowShouldClose())
    {
        if (rl::IsMouseButtonPressed(rl::MOUSE_BUTTON_RIGHT)) {
            gen.reset();
            sim.reset();
            ivec mpos = ivec(rl::GetMousePosition());
            mpos.y = rl::GetScreenHeight() - mpos.y;
            addingDangerStart = { mpos.x / rl::TILE_SIZE, mpos.y / rl::TILE_SIZE };
            if (!m.removeDangerIntersecting(addingDangerStart)) {
                addingDanger = true;
            }
        } else if (addingDanger && rl::IsMouseButtonDown(rl::MOUSE_BUTTON_RIGHT)) {
            ivec mpos = ivec(rl::GetMousePosition());
            mpos.y = rl::GetScreenHeight() - mpos.y;
            addingDangerEnd = { mpos.x / rl::TILE_SIZE, mpos.y / rl::TILE_SIZE };
        } else {
            if (addingDanger && rl::IsMouseButtonReleased(rl::MOUSE_BUTTON_RIGHT)) {
                addingDanger = false;
                m.addDanger({ addingDangerStart, addingDangerEnd });
            }

            if (rl::IsMouseButtonPressed(rl::MOUSE_BUTTON_LEFT)) {
                gen.reset();
                ivec mpos = ivec(rl::GetMousePosition());
                mpos.y = rl::GetScreenHeight() - mpos.y;
                m.setInitial({ mpos.x / rl::TILE_SIZE, mpos.y / rl::TILE_SIZE });
                sim.reset();
            }

            if (rl::IsKeyPressed(rl::KEY_RIGHT)) sim.step(DGShield::action_t::FORWARD);
            if (rl::IsKeyPressed(rl::KEY_UP)) sim.step(DGShield::action_t::UP);
            if (rl::IsKeyPressed(rl::KEY_DOWN)) sim.step(DGShield::action_t::DOWN);

            if (rl::IsKeyPressed(rl::KEY_P)) sim.finishWithRandomMoves();
            if (rl::IsKeyDown(rl::KEY_O)) sim.safeStepRandom();
            if (rl::IsKeyPressed(rl::KEY_I)) sim.reset();

            if (rl::IsKeyPressed(rl::KEY_Q)) gen.reset();
            //if (rl::IsKeyPressed(rl::KEY_W)) gen.step();
            if (rl::IsKeyPressed(rl::KEY_E)) gen.run();

            if (rl::IsKeyPressed(rl::KEY_S)) shieldRenderMode = (shield_render_mode_t)(((int)shieldRenderMode + 1) % 3);
        }

        rl::BeginDrawing();
        ClearBackground(rl::RAYWHITE);
        m.render();
        if (addingDanger) {
            irect danger(addingDangerStart, addingDangerEnd);
            rl::DrawRectangle(
                    danger.min.x * rl::TILE_SIZE,
                    (m.height - danger.max.y - 1) * rl::TILE_SIZE,
                    (danger.max.x - danger.min.x + 1) * rl::TILE_SIZE,
                    (danger.max.y - danger.min.y + 1) * rl::TILE_SIZE,
                    rl::GRAY
            );
        }
        sim.render();
        gen.render(shieldRenderMode);
        rl::EndDrawing();
    }

    rl::CloseWindow();

    return 0;
}
