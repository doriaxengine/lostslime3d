# Lost Slime 3D

A small 3D platformer made with [Doriax Engine](https://github.com/doriaxengine/doriax), a
follow-up to [Lost Slime](https://github.com/doriaxengine/lostslime).

Help the slime across two floating-island courses. Find the key to open the portal at the end
of a course (it stays locked until then), collect coins for points, look for the gems hidden
off the main path, and keep away from saws and spikes. Falling off an island costs a heart too.

Open the folder in Doriax Editor and press **Play** on `Intro Scene`.

## Controls

| Input | Action |
| --- | --- |
| WASD / arrows, gamepad stick or d-pad | Move |
| Space, gamepad A | Jump (let go early for a short hop) |
| Shift, gamepad X | Run |
| Drag the mouse, Q / E, gamepad right stick | Turn the camera |
| Esc / P, gamepad Start | Pause |
| Enter, gamepad A | Menu confirm |

On phones and tablets the level shows on-screen controls instead: a stick (push it all the way
to run), a jump button and a pause button, and dragging anywhere else turns the camera. They
also appear after touching the screen on any device, and hide again when a key is pressed.

## Project

- `Intro Scene`: the title screen, a small island with `Title Menu` as a child scene.
- `Level One`, `Level Two`: the levels. Each has a `Level` entity with the `LevelController`
  script, and uses `HUD Scene`, `Pause Scene`, `Game Over Scene` and `Win Scene` as child
  scenes.
- `Loading Scene`: shown by `SceneManager` while a scene loads.
- `bundles/`: the slime, pickups, key, portal, hazards, moving platform and the shared sounds.
- `scripts/`: the C++ scripts. `GameState` keeps the score and hearts between scenes.
- Animations: the pickups' spin and bob, the portal star, the saws, the moving platforms,
  the title camera and the loading slime are actions that loop and play on start (select
  them to see them in Properties). The portal opening and the HUD banner are Animation
  entities (open them in the Animation window) that scripts start. The menu buttons grow
  on hover through their Button settings.
- Particles: the slime's dust on jumps and landings is the `Dust Emitter` particles action of
  the Player bundle, drawing each puff as an instance of the `Dust` mesh.

The best score is saved with `System::setIntegerForKey`.

## Credits

Models, UI, skyboxes and sounds are CC0 assets by [Kenney](https://kenney.nl): Platformer Kit,
UI Pack - Adventure, Mobile Controls, Skyboxes, Interface Sounds, Impact Sounds, Music
Jingles and New Platformer Pack (the key icon and sounds). The Titan One and Sniglet fonts are
under the SIL Open Font License. Licenses are in `assets/licenses/`.

The logo, the HUD icons (renders of Platformer Kit models) and the dust puff model were made
for this game and are CC0 as well. The music was made for Lost Slime and is CC0 too.
