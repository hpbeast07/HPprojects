import { Physics } from './Physics.js';
import { Player } from './Player.js';
import { GameState } from './GameState.js';
import { LevelManager } from './LevelManager.js';

export class Game {
    constructor() {
        this.scene = new THREE.Scene();
        this.camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
        this.renderer = new THREE.WebGLRenderer({ canvas: document.getElementById('game-canvas'), antialias: true });
        this.renderer.setSize(window.innerWidth, window.innerHeight);
        this.renderer.setClearColor(0x87ceeb); // Sky blue
        this.renderer.shadowMap.enabled = true;
        
        this.physics = new Physics();
        this.player = new Player(this.scene, this.physics);
        this.state = new GameState();
        this.levelManager = new LevelManager(this.scene, this.physics);
        
        this.clock = new THREE.Clock();
        this.keys = {};
        this.lastTime = 0;
        
        this.initLights();
        this.initLevel();
        this.initEventListeners();
        
        // Load first level
        this.levelManager.loadLevel(this.state.level);
        
        // Position player at level start
        if (this.levelManager.currentLevelData && this.levelManager.currentLevelData.start) {
            this.player.reset(this.levelManager.currentLevelData.start.position);
        }
    }

    initLights() {
        const ambientLight = new THREE.AmbientLight(0xffffff, 0.6);
        this.scene.add(ambientLight);
        
        const dirLight = new THREE.DirectionalLight(0xffffff, 1);
        dirLight.position.set(20, 50, 30);
        dirLight.castShadow = true;
        dirLight.shadow.mapSize.width = 2048;
        dirLight.shadow.mapSize.height = 2048;
        dirLight.shadow.camera.near = 0.5;
        dirLight.shadow.camera.far = 50;
        dirLight.shadow.camera.left = -30;
        dirLight.shadow.camera.right = 30;
        dirLight.shadow.camera.top = 30;
        dirLight.shadow.camera.bottom = -30;
        this.scene.add(dirLight);
    }

    initLevel() {
        // REMOVED: Ground plane - now using floating tracks only
        // The level is built entirely by LevelManager
    }

    initEventListeners() {
        window.addEventListener('keydown', (e) => this.keys[e.code] = true);
        window.addEventListener('keyup', (e) => this.keys[e.code] = false);
        window.addEventListener('resize', () => this.onResize());
    }

    update() {
        // Handle game state
        if (this.state.getCurrentState() === 'PLAYING') {
            const deltaTime = Math.min(this.clock.getDelta(), 0.1);
            
            this.physics.update(deltaTime);
            this.player.update(deltaTime, this.keys);
            
            // Update level time
            this.state.levelTime += deltaTime;
            
            // Update level manager
            this.levelManager.update(deltaTime, this.player.mesh.position);
            
            // Follow camera
            const targetPos = new THREE.Vector3(
                this.player.mesh.position.x,
                this.player.mesh.position.y + 5,
                this.player.mesh.position.z + 10
            );
            this.camera.position.lerp(targetPos, 0.1);
            this.camera.lookAt(this.player.mesh.position);
            
            // Check for fall off
            if (this.player.mesh.position.y < -20) {
                this.state.failLevel();
            }
            
            // Update UI
            this.updateUI();
        }
        
        this.renderer.render(this.scene, this.camera);
        requestAnimationFrame(() => this.update());
    }

    updateUI() {
        // Update score display
        document.getElementById('score-display').textContent = `SCORE: ${Math.floor(this.state.score)}`;
        document.getElementById('coins-display').textContent = `COINS: ${this.state.coins}`;
        document.getElementById('combo-display').textContent = `COMBO: x${this.state.combo}`;
        document.getElementById('level-display').textContent = `LEVEL ${this.state.level}`;
    }

    start() {
        this.state.startLevel();
        this.clock.getDelta(); // Reset clock
        this.update();
    }

    stop() {
        // State will be handled by the App class
    }

    // Methods for state transitions (called by App)
    changeState(newState) {
        this.state.changeState(newState);
        
        // Update UI based on state
        switch(newState) {
            case 'PLAYING':
                document.getElementById('game-container').style.display = 'block';
                document.getElementById('pause-menu').style.display = 'none';
                document.getElementById('game-over-menu').style.display = 'none';
                document.getElementById('level-complete-menu').style.display = 'none';
                break;
            case 'PAUSED':
                document.getElementById('pause-menu').style.display = 'flex';
                break;
            case 'FAILED':
                document.getElementById('game-over-menu').style.display = 'flex';
                document.getElementById('final-score').textContent = Math.floor(this.state.score);
                document.getElementById('final-coins').textContent = this.state.coins;
                break;
            case 'COMPLETED':
                document.getElementById('level-complete-menu').style.display = 'flex';
                document.getElementById('level-score').textContent = Math.floor(this.state.score);
                document.getElementById('level-coins').textContent = this.state.coins;
                document.getElementById('level-time').textContent = this.state.levelTime.toFixed(2) + 's';
                document.getElementById('total-coins').textContent = this.state.totalCoins; // This would come from level data
                break;
        }
    }

    // Public methods for App to call
    pause() {
        this.state.pause();
        this.changeState('PAUSED');
    }

    resume() {
        this.state.resume();
        this.changeState('PLAYING');
    }

    restart() {
        const startPos = this.levelManager.currentLevelData?.start?.position || { x: 0, y: 5, z: 0 };
        this.player.reset(startPos);
        this.state.startLevel();
        this.changeState('PLAYING');
    }

    goToMainMenu() {
        this.state.reset();
        // This would be handled by App class
    }
}

