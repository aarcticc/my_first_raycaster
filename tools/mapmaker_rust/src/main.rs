use sdl2::event::Event;
use sdl2::keyboard::Keycode;
use sdl2::mouse::MouseButton;
use sdl2::pixels::Color;
use sdl2::rect::Rect;
use serde::{Deserialize, Serialize};
use std::fs;

const CELL_SIZE: u32 = 32;
const MAP_WIDTH: usize = 20;
const MAP_HEIGHT: usize = 20;
const WINDOW_WIDTH: u32 = MAP_WIDTH as u32 * CELL_SIZE + 200;
const WINDOW_HEIGHT: u32 = MAP_HEIGHT as u32 * CELL_SIZE;
const MAX_ENEMIES: usize = 10;

#[derive(Serialize, Deserialize, Clone, Copy)]
struct Enemy {
    x: f32,
    y: f32,
}

#[derive(Serialize, Deserialize)]
struct MapData {
    map: Vec<Vec<i32>>,
    enemies: Vec<Enemy>,
}

struct MapMaker {
    map: Vec<Vec<i32>>,
    enemies: Vec<Enemy>,
    current_texture: i32,
    mode: EditorMode,
}

#[derive(PartialEq)]
enum EditorMode {
    Wall,
    Enemy,
}

impl MapMaker {
    fn new() -> Self {
        let mut map = vec![vec![0; MAP_WIDTH]; MAP_HEIGHT];
        // Set borders
        for x in 0..MAP_WIDTH {
            map[0][x] = 1;
            map[MAP_HEIGHT - 1][x] = 1;
        }
        for y in 0..MAP_HEIGHT {
            map[y][0] = 1;
            map[y][MAP_WIDTH - 1] = 1;
        }

        MapMaker {
            map,
            enemies: Vec::new(),
            current_texture: 1,
            mode: EditorMode::Wall,
        }
    }

    fn save(&self) {
        let data = MapData {
            map: self.map.clone(),
            enemies: self.enemies.clone(),
        };

        // Save as JSON
        if let Ok(json) = serde_json::to_string_pretty(&data) {
            let _ = fs::write("map.json", json);
        }

        // Save as C code
        let mut c_code = String::from("#include \"map.h\"\n\n");
        c_code.push_str("int map[MAP_HEIGHT][MAP_WIDTH] = {\n");
        
        for row in &self.map {
            c_code.push_str("    {");
            c_code.push_str(&row.iter()
                .map(|x| x.to_string())
                .collect::<Vec<_>>()
                .join(","));
            c_code.push_str("},\n");
        }
        c_code.push_str("};\n\n");

        c_code.push_str("void spawn_initial_enemies(void) {\n");
        for enemy in &self.enemies {
            c_code.push_str(&format!("    spawn_enemy_at({:.2}f, {:.2}f);\n", 
                                   enemy.x, enemy.y));
        }
        c_code.push_str("}\n");

        let _ = fs::write("../src/map.c", c_code);
    }

    fn load(&mut self) {
        if let Ok(content) = fs::read_to_string("map.json") {
            if let Ok(data) = serde_json::from_str::<MapData>(&content) {
                self.map = data.map;
                self.enemies = data.enemies;
            }
        }
    }
}

fn main() -> Result<(), String> {
    let sdl_context = sdl2::init()?;
    let video_subsystem = sdl_context.video()?;

    let window = video_subsystem.window("Map Maker", WINDOW_WIDTH, WINDOW_HEIGHT)
        .position_centered()
        .build()
        .map_err(|e| e.to_string())?;

    let mut canvas = window.into_canvas().build().map_err(|e| e.to_string())?;
    let mut event_pump = sdl_context.event_pump()?;
    let mut mapmaker = MapMaker::new();

    'running: loop {
        canvas.set_draw_color(Color::RGB(0, 0, 0));
        canvas.clear();

        for event in event_pump.poll_iter() {
            match event {
                Event::Quit {..} => break 'running,
                Event::KeyDown { keycode: Some(key), .. } => {
                    match key {
                        Keycode::Space => {
                            mapmaker.mode = if mapmaker.mode == EditorMode::Wall {
                                EditorMode::Enemy
                            } else {
                                EditorMode::Wall
                            };
                        }
                        Keycode::S => mapmaker.save(),
                        Keycode::L => mapmaker.load(),
                        Keycode::Num1 => mapmaker.current_texture = 1,
                        Keycode::Num2 => mapmaker.current_texture = 2,
                        Keycode::Num3 => mapmaker.current_texture = 3,
                        Keycode::Num4 => mapmaker.current_texture = 4,
                        _ => {}
                    }
                }
                Event::MouseButtonDown { mouse_btn, x, y, .. } => {
                    let map_x = (x / CELL_SIZE as i32) as usize;
                    let map_y = (y / CELL_SIZE as i32) as usize;

                    if map_x < MAP_WIDTH && map_y < MAP_HEIGHT {
                        match (mouse_btn, mapmaker.mode) {
                            (MouseButton::Left, EditorMode::Wall) => {
                                mapmaker.map[map_y][map_x] = mapmaker.current_texture;
                            }
                            (MouseButton::Right, EditorMode::Wall) => {
                                mapmaker.map[map_y][map_x] = 0;
                            }
                            (MouseButton::Left, EditorMode::Enemy) => {
                                if mapmaker.enemies.len() < MAX_ENEMIES {
                                    mapmaker.enemies.push(Enemy {
                                        x: map_x as f32 + 0.5,
                                        y: map_y as f32 + 0.5,
                                    });
                                }
                            }
                            (MouseButton::Right, EditorMode::Enemy) => {
                                // Remove closest enemy
                                let click_pos = (map_x as f32 + 0.5, map_y as f32 + 0.5);
                                if let Some(idx) = mapmaker.enemies.iter().enumerate()
                                    .min_by_key(|(_, e)| {
                                        let dx = e.x - click_pos.0;
                                        let dy = e.y - click_pos.1;
                                        (dx * dx + dy * dy) as i32
                                    })
                                    .map(|(i, _)| i) {
                                    mapmaker.enemies.remove(idx);
                                }
                            }
                            _ => {}
                        }
                    }
                }
                _ => {}
            }
        }

        // Draw grid
        canvas.set_draw_color(Color::RGB(50, 50, 50));
        for i in 0..=MAP_WIDTH {
            canvas.draw_line(
                (i as i32 * CELL_SIZE as i32, 0),
                (i as i32 * CELL_SIZE as i32, WINDOW_HEIGHT as i32)
            )?;
        }
        for i in 0..=MAP_HEIGHT {
            canvas.draw_line(
                (0, i as i32 * CELL_SIZE as i32),
                ((MAP_WIDTH * CELL_SIZE as usize) as i32, i as i32 * CELL_SIZE as i32)
            )?;
        }

        // Draw walls
        for y in 0..MAP_HEIGHT {
            for x in 0..MAP_WIDTH {
                if mapmaker.map[y][x] > 0 {
                    let val = mapmaker.map[y][x] * 50;
                    canvas.set_draw_color(Color::RGB(val as u8, val as u8, 128));
                    canvas.fill_rect(Rect::new(
                        x as i32 * CELL_SIZE as i32,
                        y as i32 * CELL_SIZE as i32,
                        CELL_SIZE,
                        CELL_SIZE,
                    ))?;
                }
            }
        }

        // Draw enemies
        canvas.set_draw_color(Color::RGB(255, 0, 0));
        for enemy in &mapmaker.enemies {
            let x = (enemy.x * CELL_SIZE as f32) as i32;
            let y = (enemy.y * CELL_SIZE as f32) as i32;
            canvas.fill_rect(Rect::new(
                x - 5, y - 5, 10, 10
            ))?;
        }

        // Draw UI
        canvas.set_draw_color(Color::RGB(100, 100, 100));
        canvas.fill_rect(Rect::new(
            (MAP_WIDTH * CELL_SIZE as usize) as i32,
            0,
            200,
            WINDOW_HEIGHT,
        ))?;

        canvas.present();
    }

    Ok(())
}
