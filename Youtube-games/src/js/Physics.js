/**
 * Physics Engine wrapper using Cannon-es
 */
export class Physics {
    constructor() {
        this.world = new CANNON.World();
        this.world.gravity.set(0, -20, 0); // Stronger gravity for better feel
        this.world.broadphase = new CANNON.SAPBroadphase(this.world);
        this.world.defaultContactMaterial.friction = 0.1;
        this.world.defaultContactMaterial.restitution = 0.3;
        
        // Add a ground material
        this.groundMaterial = new CANNON.Material('groundMaterial');
        this.playerMaterial = new CANNON.Material('playerMaterial');
        
        // Define contact behavior
        const contactMaterial = new CANNON.ContactMaterial(
            this.groundMaterial,
            this.playerMaterial,
            {
                friction: 0.5,
                restitution: 0.3,
                contactEquationStiffness: 1e8,
                contactEquationRelaxation: 3
            }
        );
        this.world.addContactMaterial(contactMaterial);
    }

    update(deltaTime) {
        // Use a fixed time step for stability
        this.world.step(1 / 60, deltaTime, 3);
    }

    addBody(body) {
        this.world.addBody(body);
    }

    removeBody(body) {
        this.world.removeBody(body);
    }
}
