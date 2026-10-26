import { LevelData } from './LevelData.js';

export class LevelManager {
    constructor(scene, physics) {
        this.scene = scene;
        this.physics = physics;
        this.physicsWorld = physics.world;
        this.currentLevelData = null;
        this.trackMeshes = [];
        this.obstacleBodies = [];
        this.coinMeshes = [];
        this.checkpointMeshes = [];
        this.finishMesh = null;
        this.specialObjects = []; // Track objects needing updates (moving, rotating, falling)
        
        // Materials
        this.materials = {
            track: new THREE.MeshStandardMaterial({ color: 0x444444, roughness: 0.5 }),
            obstacle: new THREE.MeshStandardMaterial({ color: 0xff0000, roughness: 0.5 }),
            coin: new THREE.MeshStandardMaterial({ color: 0xffd700, metalness: 1.0, roughness: 0.3 }),
            checkpoint: new THREE.MeshStandardMaterial({ color: 0x00ff00, transparent: true, opacity: 0.5 }),
            finish: new THREE.MeshStandardMaterial({ color: 0xffff00, emissive: 0xffff00, emissiveIntensity: 0.5 }),
            ramp: new THREE.MeshStandardMaterial({ color: 0x884444, roughness: 0.3 }),
            boost: new THREE.MeshStandardMaterial({ color: 0x0088ff, emissive: 0x004488, emissiveIntensity: 0.5 })
        };
    }

    loadLevel(levelNumber) {
        this.clearLevel();
        this.currentLevelData = LevelData.getLevelData(levelNumber);
        
        this.buildTrack();
        this.placeCoins();
        this.placeCheckpoints();
        this.placeObstacles();
        this.placeFinish();
    }

    buildTrack() {
        if (!this.currentLevelData) return;

        this.currentLevelData.track.forEach(segment => {
            let geometry, material;
            
            // Determine geometry and material based on segment type
            switch(segment.type) {
                case "ramp":
                    geometry = new THREE.BoxGeometry(segment.size.x, segment.size.y, segment.size.z);
                    material = this.materials.ramp;
                    break;
                case "speed_ramp":
                    geometry = new THREE.BoxGeometry(segment.size.x, segment.size.y, segment.size.z);
                    material = this.materials.boost;
                    break;
                case "narrow_platform":
                    geometry = new THREE.BoxGeometry(segment.size.x, segment.size.y, segment.size.z);
                    material = this.materials.track;
                    break;
                case "moving_platform":
                    geometry = new THREE.BoxGeometry(segment.size.x, segment.size.y, segment.size.z);
                    material = this.materials.track;
                    break;
                case "falling_platform":
                    geometry = new THREE.BoxGeometry(segment.size.x, segment.size.y, segment.size.z);
                    material = this.materials.track;
                    break;
                default: // straight, gap, etc.
                    geometry = new THREE.BoxGeometry(segment.size.x, segment.size.y, segment.size.z);
                    material = this.materials.track;
                    // For gaps, we'll create invisible physics bodies later
                    if (segment.type === "gap") {
                        material.opacity = 0;
                        material.transparent = true;
                    }
            }
            
            const mesh = new THREE.Mesh(geometry, material);
            
            const pos = segment.position || { x: 0, y: 0, z: 0 };
            mesh.position.set(pos.x, pos.y, pos.z);
            
            const rot = segment.rotation || { x: 0, y: 0, z: 0 };
            mesh.rotation.set(rot.x, rot.y, rot.z);
            
            mesh.receiveShadow = true;
            mesh.castShadow = true;
            this.scene.add(mesh);
            this.trackMeshes.push(mesh);

            // Physics body (skip for visual gaps)
            if (segment.type !== "gap") {
                const shape = new CANNON.Box(new CANNON.Vec3(
                    segment.size.x / 2, 
                    segment.size.y / 2, 
                    segment.size.z / 2
                ));
                
                const body = new CANNON.Body({ 
                    mass: 0, 
                    material: this.physics.groundMaterial 
                });
                
                body.addShape(shape);
                body.position.set(pos.x, pos.y, pos.z);
                
                if (segment.rotation) {
                    body.quaternion.setFromEuler(
                        segment.rotation.x || 0, 
                        segment.rotation.y || 0, 
                        segment.rotation.z || 0
                    );
                }
                
                this.physicsWorld.addBody(body);
                this.obstacleBodies.push({ body, segment });
            }
        });
    }

    placeCoins() {
        if (!this.currentLevelData) return;

        this.currentLevelData.coins.forEach(coin => {
            const geometry = new THREE.CylinderGeometry(0.5, 0.5, 0.1, 16);
            const mesh = new THREE.Mesh(geometry, this.materials.coin);
            mesh.position.set(coin.x, coin.y + 1, coin.z);
            mesh.rotation.x = Math.PI / 2;
            this.scene.add(mesh);
            this.coinMeshes.push(mesh);
        });
    }

    placeCheckpoints() {
        if (!this.currentLevelData) return;

        this.currentLevelData.checkpoints.forEach(cp => {
            const geometry = new THREE.BoxGeometry(5, 5, 1);
            const mesh = new THREE.Mesh(geometry, this.materials.checkpoint);
            mesh.position.set(cp.x, cp.y + 2.5, cp.z);
            this.scene.add(mesh);
            this.checkpointMeshes.push(mesh);
        });
    }

    placeObstacles() {
        if (!this.currentLevelData || !this.currentLevelData.obstacles) return;

        this.currentLevelData.obstacles.forEach(obs => {
            let geometry, material;
            
            // Determine geometry and material based on obstacle type
            switch(obs.type) {
                case "spinner":
                case "rotating_bar":
                    geometry = new THREE.BoxGeometry(obs.size.x, obs.size.y, obs.size.z);
                    material = this.materials.obstacle;
                    break;
                case "wall":
                case "moving_platform":
                case "falling_platform":
                    geometry = new THREE.BoxGeometry(obs.size.x, obs.size.y, obs.size.z);
                    material = this.materials.obstacle;
                    break;
                default:
                    geometry = new THREE.BoxGeometry(obs.size.x, obs.size.y, obs.size.z);
                    material = this.materials.obstacle;
            }
            
            const mesh = new THREE.Mesh(geometry, material);
            
            const pos = obs.position || { x: 0, y: 0, z: 0 };
            mesh.position.set(pos.x, pos.y, pos.z);
            
            // Set rotation if specified
            if (obs.rotation) {
                mesh.rotation.set(obs.rotation.x || 0, obs.rotation.y || 0, obs.rotation.z || 0);
            }

            mesh.castShadow = true;
            mesh.receiveShadow = true;
            this.scene.add(mesh);
            this.trackMeshes.push(mesh);

            // Physics body
            const shape = new CANNON.Box(new CANNON.Vec3(
                obs.size.x / 2, 
                obs.size.y / 2, 
                obs.size.z / 2
            ));
            
            let mass = 0; // Static by default
            if (obs.type === "falling_platform") {
                mass = 0; // Will be changed when falling starts
            }
            
            const body = new CANNON.Body({ 
                mass: mass,
                material: this.physics.groundMaterial 
            });
            
            body.addShape(shape);
            body.position.set(pos.x, pos.y, pos.z);
            
            // Set rotation if specified
            if (obs.rotation) {
                body.quaternion.setFromEuler(
                    obs.rotation.x || 0, 
                    obs.rotation.y || 0, 
                    obs.rotation.z || 0
                );
            }
            
            this.physicsWorld.addBody(body);
            
            // Store special object data for updates
            this.specialObjects.push({ 
                mesh, 
                body, 
                obs 
            });
        });
    }

    placeFinish() {
        if (!this.currentLevelData) return;
        
        const finishPos = this.currentLevelData.finish.position;
        const geometry = new THREE.TorusGeometry(3, 0.5, 16, 32);
        this.finishMesh = new THREE.Mesh(geometry, this.materials.finish);
        this.finishMesh.position.set(finishPos.x, finishPos.y + 3, finishPos.z);
        this.scene.add(this.finishMesh);
    }

    update(deltaTime, playerPosition) {
        // Rotate coins
        this.coinMeshes.forEach(mesh => {
            mesh.rotation.z += deltaTime * 2;
        });
        
        // Simple collision detection for coins
        for (let i = this.coinMeshes.length - 1; i >= 0; i--) {
            const coin = this.coinMeshes[i];
            if (coin.position.distanceTo(playerPosition) < 1.5) {
                this.scene.remove(coin);
                this.coinMeshes.splice(i, 1);
                // Dispatch event or callback for coin collection
                window.dispatchEvent(new CustomEvent('coinCollected'));
            }
        }
        
        // Check for finish line
        if (this.finishMesh && this.finishMesh.position.distanceTo(playerPosition) < 3) {
            window.dispatchEvent(new CustomEvent('levelComplete'));
        }

        // Update special objects (moving platforms, rotating obstacles, falling platforms)
        this.updateSpecialObjects(deltaTime, playerPosition);
    }

    updateSpecialObjects(deltaTime, playerPosition) {
        this.specialObjects.forEach(obj => {
            const obs = obj.obs;
            const mesh = obj.mesh;
            const body = obj.body;
            const time = performance.now() / 1000; // Convert to seconds
            
            switch(obs.type) {
                case "moving_platform":
                    this.updateMovingPlatform(obs, mesh, body, time);
                    break;
                    
                case "spinner":
                case "rotating_bar":
                    this.updateRotatingObstacle(obs, mesh, body, time);
                    break;
                    
                case "falling_platform":
                    this.updateFallingPlatform(obs, mesh, body, playerPosition, time);
                    break;
            }
        });
    }

    updateMovingPlatform(obs, mesh, body, time) {
        if (!obs.movementAxis || !obs.movementRange || !obs.movementSpeed) return;
        
        let offset = 0;
        
        if (obs.movementAxis === "x") {
            offset = Math.sin(time * obs.movementSpeed) * obs.movementRange;
            body.position.x = obs.position.x + offset;
            mesh.position.x = body.position.x;
        } else if (obs.movementAxis === "z") {
            offset = Math.sin(time * obs.movementSpeed) * obs.movementRange;
            body.position.z = obs.position.z + offset;
            mesh.position.z = body.position.z;
        }
        
        // Keep Y position constant
        body.position.y = obs.position.y;
        mesh.position.y = body.position.y;
    }

    updateRotatingObstacle(obs, mesh, body, time) {
        if (!obs.rotationSpeed) return;
        
        const angle = time * obs.rotationSpeed * (Math.PI * 2); // Convert to radians
        body.quaternion.setFromEuler(0, angle, 0);
        mesh.rotation.y = angle;
    }

    updateFallingPlatform(obs, mesh, body, playerPosition, time) {
        if (!obs.fallDelay) obs.fallDelay = 0.5;
        if (!obs.fallSpeed) obs.fallSpeed = 5;
        
        // Simple distance check for player proximity
        const distance = Math.sqrt(
            Math.pow(playerPosition.x - obs.position.x, 2) +
            Math.pow(playerPosition.z - obs.position.z, 2)
        );
        
        // If player is close and platform hasn't started falling
        if (distance < 3 && !obs.isFalling && !obs.hasFallen) {
            obs.isFalling = true;
            obs.fallStartTime = performance.now();
            
            // Visual warning: flash the platform
            if (mesh.material) {
                mesh.material.emissive.setHex(0xff0000);
                setTimeout(() => {
                    if (mesh.material) mesh.material.emissive.setHex(0x000000);
                }, 100);
            }
        }
        
        // If falling, apply gravity after delay
        if (obs.isFalling && !obs.hasFallen) {
            const timeElapsed = (performance.now() - obs.fallStartTime) / 1000;
            
            if (timeElapsed > obs.fallDelay) {
                // Enable gravity
                body.mass = obs.mass || 1;
                body.velocity.set(0, -obs.fallSpeed, 0);
                obs.hasFallen = true;
                
                // Change material to indicate fallen
                if (mesh.material) {
                    mesh.material.color.setHex(0x555555);
                }
            }
        }
    }

    

    }
    


    clearLevel() {
        // Remove track meshes
        this.trackMeshes.forEach(mesh => {
            this.scene.remove(mesh);
            mesh.geometry.dispose();
            // Don't dispose materials as they are shared
        });
        this.trackMeshes = [];
        
        // Remove obstacle bodies
        this.obstacleBodies.forEach(body => {
            this.physicsWorld.removeBody(body);
        });
        this.obstacleBodies = [];
        
        // Remove coin meshes
        this.coinMeshes.forEach(mesh => {
            this.scene.remove(mesh);
            mesh.geometry.dispose();
            // Don't dispose materials as they are shared
        });
        this.coinMeshes = [];
        
        // Remove checkpoint meshes
        this.checkpointMeshes.forEach(mesh => {
            this.scene.remove(mesh);
            mesh.geometry.dispose();
            // Don't dispose materials as they are shared
        });
        this.checkpointMeshes = [];
        
        // Remove special objects (meshes and bodies)
        this.specialObjects.forEach(obj => {
            if (obj.mesh) {
                this.scene.remove(obj.mesh);
                obj.mesh.geometry.dispose();
                // Don't dispose materials as they are shared
            }
            if (obj.body) {
                this.physicsWorld.removeBody(obj.body);
            }
        });
        this.specialObjects = [];

        // Remove finish mesh
        if (this.finishMesh) {
            this.scene.remove(this.finishMesh);
            this.finishMesh.geometry.dispose();
            // Don't dispose materials as they are shared
            this.finishMesh = null;
        }
    }