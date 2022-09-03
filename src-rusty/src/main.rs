pub mod structs; 

use std::fs::File;
use std::io::prelude::*;
use std::io::BufReader;
use std::collections::HashMap;
use serde_json::{Value, Error};
use structs::{Vec3, Spawn, Scene}; 
use indicatif::{ProgressBar, ProgressStyle};

fn parse_vector3(d: &Value) -> Vec3 {
    Vec3 {
        x: d["x"].as_f64().unwrap(),
        y: d["y"].as_f64().unwrap(),
        z: d["z"].as_f64().unwrap(),
    }
}

fn parse_spawn(d: &Value) -> Spawn {
    Spawn {
        monster_id: d["monsterId"].as_i64().unwrap(),
        config_id: d["configId"].as_i64().unwrap(),
        level: d["level"].as_i64().unwrap(),
        pose_id: d["poseId"].as_i64().unwrap(),
        pos: parse_vector3(&d["pos"]),
        rot: parse_vector3(&d["rot"]),
    }
}

fn parse_scene(d: &Value) -> Scene {
    Scene {
        scene_id: d["sceneId"].as_i64().unwrap(),
        group_id: d["groupId"].as_i64().unwrap(),
        block_id: d["blockId"].as_i64().unwrap(),
        pos: parse_vector3(&d["pos"]),
        spawns: d["spawns"].as_array().unwrap().iter().map(|spawn| parse_spawn(spawn)).collect(),
    }
}

fn main() -> Result<(), Error> {
    let pb = ProgressBar::new(100);
    pb.set_style(ProgressStyle::default_bar().template("[{elapsed_precise}] {bar:40.cyan/blue} {pos:>7}/{len:7} {msg}").unwrap());
    
    let file = File::open("../../data/Spawns.json").unwrap(); 
    let reader = BufReader::new(file);
    let json: Value = serde_json::from_reader(reader)?;
    
    let mut scenes: Vec<Scene> = Vec::new(); 
    pb.set_message("Parsing scenes");
    for scene in json.as_array().unwrap() {
        scenes.push(parse_scene(scene));
    }
    
    let mut parsed_spawns: HashMap<String, Vec<Vec3>> = HashMap::new();
    pb.set_message("Parsing spawns");
    for scene in scenes {
        for spawn in scene.spawns {
            if !parsed_spawns.contains_key(&spawn.monster_id.to_string()) {
                parsed_spawns.insert(spawn.monster_id.to_string(), Vec::new());
            } else {
                parsed_spawns.get_mut(&spawn.monster_id.to_string()).unwrap().push(spawn.pos)
            }
        }
    }
    
    pb.set_message("Writing to file");
    let mut file = File::create("../../parsed/parsedSpawns.json").unwrap();
    file.write(serde_json::to_string(&parsed_spawns).unwrap().as_bytes()).unwrap();
    
    pb.finish_with_message("Done");
    Ok(())
}