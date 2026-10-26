/**
 * Player class representing the ball
 */
export class Player {
    constructor(scene, physics) {
        this.scene = scene;
        this.physics = physics;
        this.physicsWorld = physics.world;
        
        // Three.js mesh
        this.mesh = null;
        
        // Cannon.js body
        this.body = null;
        
        this.radius = 0.8;
        this.init();
    }
    
    init() {
        // Create the sphere geometry and material
        const geometry = new THREE.SphereGeometry(this.radius, 32, 32);
        const material = new THREE.MeshStandardMaterial({ 
            color: 0xffffff,
            metalness: 0.1,
            roughness: 0.5
        });
        this.mesh = new THREE.Mesh(geometry, material);
        this.mesh.castShadow = true;
        this.mesh.name = 'player'; // For identification
        this.scene.add(this.mesh);
        
        // Create Cannon.js body
        const shape = new CANNON.Sphere(this.radius);
        this.body = new CANNON.Body({
            mass: 1,
            shape: shape,
            material: this.physics.playerMaterial,
            linearDamping: 0.1,
            angularDamping: 0.1
        });
        this.body.position.set(0, 5, 0); // Starting position
        this.physicsWorld.addBody(this.body);
    }
    
    /**
     * Update player based on input and physics
     * @param {number} deltaTime - Time since last frame
     * @param {Object} keys - Current keyboard state
     */
    update(deltaTime, keys) {
        // Handle input
        const input = this.getInput(keys);
        
        // Apply force for movement
        if (input.x !== 0 || input.z !== 0) {
            const force = new CANNON.Vec3(input.x, 0, input.z).normalize().scale(20);
            this.body.applyForce(force, this.body.position);
        }
        
        // Handle jump
        if (input.jump && this.grounded()) {
            this.body.applyImpulse(new CANNON.Vec3(0, 8, 0), this.body.position);
        }
        
        // Sync Three.js mesh with Cannon.js body
        this.mesh.position.copy(this.body.position);
        this.mesh.quaternion.copy(this.body.quaternion);
    }
    
    /**
     * Get input from keyboard state
     * @param {Object} keys - Current keyboard state
     * @returns {{x: number, z: number, jump: boolean}}
     */
    getInput(keys) {
        return {
            x: (keys['KeyD'] || keys['ArrowRight'] ? 1 : 0) - (keys['KeyA'] || keys['ArrowLeft'] ? 1 : 0),
            z: (keys['KeyS'] || keys['ArrowDown'] ? 1 : 0) - (keys['KeyW'] || keys['ArrowUp'] ? 1 : 0),
            jump: keys['Space']
        };
    }
    
    /**
     * Check if the player is grounded
     * @returns {boolean}
     */
    grounded() {
        const rayStart = new CANNON.Vec3(
            this.body.position.x,
            this.body.position.y,
            this.body.position.z
        );
        const rayEnd = new CANNON.Vec3(
            this.body.position.x,
            this.body.position.y - this.radius - 0.2,
            this.body.position.z
        );
        
        const result = new CANNON.RaycastResult();
        const hasHit = this.physicsWorld.raycastClosest(rayStart, rayEnd, {}, result);
        return hasHit;
    }
    
    /**
     * Reset player to starting position
     */
    reset(startPosition = { x: 0, y: 5, z: 0 }) {
        this.body.position.set(startPosition.x, startPosition.y, startPosition.z);
        this.body.velocity.set(0, 0, 0);
        this.body.angularVelocity.set(0, 0, 0);
        this.mesh.position.copy(this.body.position);
        this.mesh.quaternion.copy(this.body.quaternion);
    }
}
