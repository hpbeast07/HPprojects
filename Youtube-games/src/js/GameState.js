export class GameState {
    constructor() {
        this.state = 'MENU'; // MENU, PLAYING, PAUSED, FAILED, COMPLETED
        this.previousState = null;
        this.level = 1;
        this.score = 0;
        this.coins = 0;
        this.combo = 1;
        this.bestScore = 0;
        this.bestTime = 0;
        this.levelTime = 0;
        this.totalCoins = 0;
        this.isGrounded = false;
        this.hasCheckpoint = false;
        this.checkpointPosition = null;
        this.checkpointRotation = null;
    }

    changeState(newState) {
        this.previousState = this.state;
        this.state = newState;
    }

    reset() {
        this.state = 'MENU';
        this.previousState = null;
        this.level = 1;
        this.score = 0;
        this.coins = 0;
        this.combo = 1;
        this.levelTime = 0;
        this.totalCoins = 0;
        this.isGrounded = false;
        this.hasCheckpoint = false;
        this.checkpointPosition = null;
        this.checkpointRotation = null;
    }

    startLevel() {
        this.changeState('PLAYING');
        this.levelTime = 0;
        this.coins = 0;
        this.combo = 1;
        this.isGrounded = false;
        this.hasCheckpoint = false;
        this.checkpointPosition = null;
        this.checkpointRotation = null;
    }

    completeLevel() {
        this.changeState('COMPLETED');
        // Update best score and time if applicable
        if (this.score > this.bestScore) {
            this.bestScore = this.score;
        }
        if (this.levelTime < this.bestTime || this.bestTime === 0) {
            this.bestTime = this.levelTime;
        }
    }

    failLevel() {
        this.changeState('FAILED');
    }

    pause() {
        if (this.state === 'PLAYING') {
            this.changeState('PAUSED');
        }
    }

    resume() {
        if (this.state === 'PAUSED') {
            this.changeState('PLAYING');
        }
    }

    // Getters and setters
    getCurrentState() {
        return this.state;
    }

    isPlaying() {
        return this.state === 'PLAYING';
    }

    isPaused() {
        return this.state === 'PAUSED';
    }

    isMenu() {
        return this.state === 'MENU';
    }

    isFailed() {
        return this.state === 'FAILED';
    }

    isCompleted() {
        return this.state === 'COMPLETED';
    }
}