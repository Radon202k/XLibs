void ship_draw(void)
{
    // Draw ship
    xsprite(engine.layer1, engine.arrow, 
            gs.ship.pos, gs.ship.dim, sap4f,
            ini2f(.5f,.5f), gs.ship.rot, 0);
    
    Circlef shiphitbox = {gs.ship.pos, gs.ship.dim.x*.5f};
    xlinecircle(shiphitbox.center, shiphitbox.radius, 20, eme4f, 20);
}