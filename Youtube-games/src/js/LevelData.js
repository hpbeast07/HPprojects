// Level data structure - each level is defined by this format
export class LevelData {
    static getLevelData(levelNumber) {
        // Return a structured object for the level
        const levelData = {
            levelNumber: levelNumber,
            name: `Level ${levelNumber}`,
            description: "",
            start: {
                position: { x: 0, y: 5, z: 0 },
                rotation: { x: 0, y: 0, z: 0 }
            },
            track: [],
            coins: [],
            checkpoints: [],
            obstacles: [],
            finish: { position: { x: 0, y: 2, z: 0 } }, // will be set based on track
            timeTarget: 30,
            coinTarget: 10,
            threeStars: {
                maxTime: 30,
                minCoins: 10,
                maxFalls: 0
            }
        };

        // Populate track based on level number
        if (levelNumber === 1) {
            // LEVEL 1 — TUTORIAL
            // Teach: movement, jump, coins, checkpoint, finish
            levelData.name = "Tutorial";
            levelData.description = "Learn the basics: move, jump, collect coins, hit checkpoints, and reach the finish.";
            levelData.timeTarget = 45;
            levelData.coinTarget = 6;
            levelData.threeStars = { maxTime: 30, minCoins: 6, maxFalls: 0 };
            
            levelData.track.push(
                { type: "straight", size: { x: 6, y: 1, z: 15 }, position: { x: 0, y: 0, z: 7.5 }, rotation: { x: 0, y: 0, z: 0 } },
                { type: "straight", size: { x: 6, y: 1, z: 10 }, position: { x: 0, y: 0, z: 25 }, rotation: { x: 0, y: 0, z: 0 } },
                { type: "ramp", size: { x: 4, y: 2, z: 10 }, position: { x: 0, y: 0, z: 35 }, rotation: { x: 0, y: 0, z: 0 } },
                { type: "straight", size: { x: 6, y: 2, z: 10 }, position: { x: 0, y: 2, z: 45 }, rotation: { x: 0, y: 0, z: 0 } },
                { type: "gap", size: { x: 3, y: 2, z: 10 }, position: { x: 0, y: 2, z: 55 }, rotation: { x: 0, y: 0, z: 0 } },
                { type: "straight", size: { x: 6, y: 1, z: 10 }, position: { x: 0, y: 0, z: 65 }, rotation: { x: 0, y: 0, z: 0 } }
            );
            
            levelData.coins.push(
                { x: 0, y: 1, z: 10 },
                { x: 0, y: 1, z: 20 },
                { x: 0, y: 1, z: 30 }, // on ramp
                { x: 0, y: 3, z: 40 }, // after ramp
                { x: 0, y: 1, z: 50 }, // after gap
                { x: 0, y: 1, z: 60 }  // near end
            );
            
            levelData.checkpoints.push(
                { x: 0, y: 0.5, z: 20 }, // before ramp
                { x: 0, y: 0.5, z: 50 }  // after gap
            );
            
            levelData.obstacles.push(
                { type: "wall", position: { x: 0, y: 1, z: 45 }, size: { x: 2, y: 2, z: 0.5 }, rotationSpeed: 0, movementRange: 0 }
            );
            
            // Set finish at the end of the track
            levelData.finish.position = { x: 0, y: 2, z: 70 };
        } else if (levelNumber === 2) {
            // LEVEL 2 — MOVING SKY
            // Introduce: moving platforms, gaps, timing sections
            levelData.name = "Moving Sky";
            levelData.description = "Moving platforms require timing. Watch your step!";
            levelData.timeTarget = 40;
            levelData.coinTarget = 8;
            levelData.threeStars = { maxTime: 25, minCoins: 8, maxFalls: 1 };
            
            levelData.track.push(
                { type: "straight", size: { x: 6, y: 1, z: 10 }, position: { x: 0, y: 0, z: 5 }, rotation: { x: 0, y: 0, z: 0 } },
                { type: "straight", size: { x: 6, y: 1, z: 10 }, position: { x: 0, y: 0, z: 15 }, rotation: { x: 0, y: 0, z: 0 } }
            );
            
            levelData.coins.push(
                { x: 0, y: 1, z: 8 },
                { x: -2, y: 1, z: 20 }, // on moving platform path
                { x: 2, y: 1, z: 20 },  // on moving platform path
                { x: 0, y: 1, z: 25 },
                { x: 0, y: 1, z: 35 }
            );
            
            levelData.checkpoints.push(
                { x: 0, y: 0.5, z: 10 },
                { x: 0, y: 0.5, z: 30 }
            );
            
            levelData.obstacles.push(
                { 
                    type: "moving_platform", 
                    position: { x: 0, y: 1, z: 20 }, 
                    size: { x: 4, y: 1, z: 4 }, 
                    rotationSpeed: 0, 
                    movementRange: 3, 
                    movementSpeed: 2, 
                    movementAxis: "x" 
                }
            );
            
            // Set finish at the end of the track
            levelData.finish.position = { x: 0, y: 2, z: 40 };
        } else if (levelNumber === 3) {
            // LEVEL 3 — SPEEDWAY
            // Introduce: speed ramps, boost sections, faster gameplay, long jumps
            levelData.name = "Speedway";
            levelData.description = "Speed ramps boost your velocity. Time your jumps!";
            levelData.timeTarget = 25;
            levelData.coinTarget = 10;
            levelData.threeStars = { maxTime: 18, minCoins: 10, maxFalls: 1 };
            
            levelData.track.push(
                { type: "straight", size: { x: 6, y: 1, z: 20 }, position: { x: 0, y: 0, z: 10 }, rotation: { x: 0, y: 0, z: 0 } },
                { type: "speed_ramp", size: { x: 6, y: 1, z: 15 }, position: { x: 0, y: 0, z: 25 }, rotation: { x: 0, y: 0, z: 0 } },
                { type: "straight", size: { x: 8, y: 1, z: 30 }, position: { x: 0, y: 2, z: 45 }, rotation: { x: 0, y: 0, z: 0 } }, // Long jump section
                { type: "straight", size: { x: 6, y: 1, z: 20 }, position: { x: 0, y: 0, z: 80 }, rotation: { x: 0, y: 0, z: 0 } }
            );
            
            levelData.coins.push(
                { x: 0, y: 1, z: 15 },
                { x: 0, y: 1, z: 30 }, // on speed ramp
                { x: -3, y: 3, z: 50 }, // air coin for jump
                { x: 3, y: 3, z: 50 },  // air coin for jump
                { x: 0, y: 1, z: 60 },
                { x: 0, y: 1, z: 70 },
                { x: 0, y: 1, z: 75 }
            );
            
            levelData.checkpoints.push(
                { x: 0, y: 0.5, z: 15 },
                { x: 0, y: 0.5, z: 60 }
            );
            
            levelData.obstacles.push(
                { type: "wall", position: { x: 0, y: 1, z: 40 }, size: { x: 6, y: 2, z: 0.5 }, rotationSpeed: 0, movementRange: 0 } // Barrier to jump
            );
            
            // Set finish at the end of the track
            levelData.finish.position = { x: 0, y: 2, z: 90 };
        } else if (levelNumber === 5) {
            // LEVEL 5 — SKY BRIDGE
            // Introduce: very narrow platforms, large gaps, precision movement
            levelData.name = "Sky Bridge";
            levelData.description = "Narrow paths require precision. One wrong move and you fall!";
            levelData.timeTarget = 50;
            levelData.coinTarget = 10;
            levelData.threeStars = { maxTime: 35, minCoins: 10, maxFalls: 2 };
            
            levelData.track.push(
                { type: "straight", size: { x: 6, y: 1, z: 10 }, position: { x: 0, y: 0, z: 5 }, rotation: { x: 0, y: 0, z: 0 } },
                { type: "narrow_platform", size: { x: 2, y: 1, z: 20 }, position: { x: 0, y: 0, z: 20 }, rotation: { x: 0, y: 0, z: 0 } },
                { type: "gap", size: { x: 2, y: 1, z: 30 }, position: { x: 0, y: 0, z: 40 }, rotation: { x: 0, y: 0, z: 0 } }, // Large gap
                { type: "narrow_platform", size: { x: 2, y: 1, z: 20 }, position: { x: 0, y: 0, z: 70 }, rotation: { x: 0, y: 0, z: 0 } },
                { type: "straight", size: { x: 6, y: 1, z: 10 }, position: { x: 0, y: 0, z: 90 }, rotation: { x: 0, y: 0, z: 0 } }
            );
            
            levelData.coins.push(
                { x: 0, y: 1, z: 8 },
                { x: 0, y: 1, z: 15 }, // on narrow platform
                { x: 0, y: 1, z: 30 }, // before gap (risky)
                { x: 0, y: 1, z: 60 }, // on narrow platform
                { x: 0, y: 1, z: 75 }, // on narrow platform
                { x: 0, y: 1, z: 85 },
                { x: 0, y: 1, z: 95 }
            );
            
            levelData.checkpoints.push(
                { x: 0, y: 0.5, z: 15 }, // before narrow
                { x: 0, y: 0.5, z: 50 }, // before gap
                { x: 0, y: 0.5, z: 80 }  // after narrow
            );
            
            // No obstacles, just precision platforming
            
            // Set finish at the end of the track
            levelData.finish.position = { x: 0, y: 2, z: 100 };
        }

        return levelData;
    }
}
