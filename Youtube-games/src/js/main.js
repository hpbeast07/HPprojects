// Entry point for the game
import { Game } from './Game.js';

class App {
    constructor() {
        this.game = null;
        this.isInitialized = false;
        this.initUI();
        this.hideLoading();
    }

    initUI() {
        // Main Menu Buttons
        document.getElementById('play-btn').addEventListener('click', () => this.startGame());
        document.getElementById('levels-btn').addEventListener('click', () => this.showLevels());
        
        // Navigation
        document.getElementById('back-to-main-from-levels').addEventListener('click', () => this.showMainMenu());
        document.getElementById('quit-to-main-btn').addEventListener('click', () => this.quitToMain());
        
        // Game UI
        document.getElementById('resume-btn').addEventListener('click', () => this.resumeGame());
        document.getElementById('restart-btn').addEventListener('click', () => this.restartGame());
        document.getElementById('try-again-btn').addEventListener('click', () => this.restartGame());
        document.getElementById('next-level-btn').addEventListener('click', () => this.nextLevel());
        document.getElementById('replay-level-btn').addEventListener('click', () => this.restartGame());
        document.getElementById('level-select-complete-btn').addEventListener('click', () => this.showLevels());
        document.getElementById('level-select-btn').addEventListener('click', () => this.showLevels());
        
        // Pause button (would be implemented in UI overlay)
        // This would normally be a button in the UI overlay
    }

    hideLoading() {
        setTimeout(() => {
            const loadingScreen = document.getElementById('loading-screen');
            loadingScreen.style.opacity = '0';
            setTimeout(() => {
                loadingScreen.style.display = 'none';
                this.showMainMenu();
            }, 500);
        }, 1500);
    }

    showMainMenu() {
        this.hideAllMenus();
        document.getElementById('main-menu').style.display = 'flex';
        
        // Stop game if running
        if (this.game && this.game.state.getCurrentState() === 'PLAYING') {
            this.game.stop();
        }
    }

    showLevels() {
        this.hideAllMenus();
        document.getElementById('level-select-menu').style.display = 'flex';
        this.populateLevelGrid();
    }

    startGame() {
        this.hideAllMenus();
        document.getElementById('game-container').style.display = 'block';
        
        if (!this.game) {
            this.game = new Game();
        }
        
        // Reset and start level 1
        this.game.state.reset();
        this.game.state.level = 1;
        this.game.start();
    }

    resumeGame() {
        document.getElementById('pause-menu').style.display = 'none';
        this.game.start(); // This will resume the update loop
    }

    restartGame() {
        this.hideAllMenus();
        document.getElementById('game-container').style.display = 'block';
        
        if (this.game) {
            this.game.state.reset();
            this.game.state.level = 1; // Reset to level 1 for now
            this.game.player.reset();
            this.game.start();
        }
    }

    quitToMain() {
        if (this.game) {
            this.game.stop();
        }
        this.showMainMenu();
    }

    nextLevel() {
        // For now, just restart the same level
        // In a full implementation, this would increment the level
        this.restartGame();
    }

    hideAllMenus() {
        const menus = ['main-menu', 'level-select-menu', 'pause-menu', 'game-over-menu', 'level-complete-menu', 'game-container'];
        menus.forEach(id => {
            const el = document.getElementById(id);
            if (el) el.style.display = 'none';
        });
    }

    populateLevelGrid() {
        const levelsGrid = document.getElementById('levels-grid');
        levelsGrid.innerHTML = '';
        
        // Create 10 level cards for demonstration
        for (let i = 1; i <= 10; i++) {
            const levelCard = document.createElement('div');
            levelCard.className = 'level-card';
            levelCard.dataset.level = i;
            
            // For now, all levels are unlocked except level 1
            if (i > 1) {
                levelCard.classList.add('locked');
            }
            
            levelCard.innerHTML = `
                <div class="level-content">
                    <div class="level-number">${i}</div>
                    <div class="level-stars">
                        <!-- Stars will be dynamically filled based on saved data -->
                        <span class="star empty"></span>
                        <span class="star empty"></span>
                        <span class="star empty"></span>
                    </div>
                    <div class="level-info">
                        <span>Best: 0s</span>
                        <span>Coins: 0/0</span>
                    </div>
                </div>
                <div class="locked-overlay">
                    🔒 LOCKED
                </div>
            `;
            
            // Add click handler for unlocked levels
            if (i === 1) {
                levelCard.addEventListener('click', () => this.selectLevel(i));
            }
            
            levelsGrid.appendChild(levelCard);
        }
    }

    selectLevel(levelNumber) {
        this.hideAllMenus();
        document.getElementById('game-container').style.display = 'block';
        
        if (!this.game) {
            this.game = new Game();
        }
        
        // Set up the selected level
        this.game.state.reset();
        this.game.state.level = levelNumber;
        this.game.player.reset();
        this.game.start();
    }
}

// Wait for the DOM to load
document.addEventListener('DOMContentLoaded', () => {
    window.app = new App();
});
