class House {
    constructor() {
        this.x = 300;
        this.y = 300;
        this.width = 200;
        this.height = 150;
    }

    draw(ctx) {
        // Дом
        ctx.fillStyle = '#CD853F';
        ctx.fillRect(this.x, this.y, this.width, this.height);

        // Труба
        ctx.fillStyle = '#696969';
        ctx.fillRect(this.x + this.width - 50, this.y - 50, 30, 50);

        // Крыша
        ctx.beginPath();
        ctx.moveTo(this.x - 20, this.y);
        ctx.lineTo(this.x + this.width/2, this.y - 60);
        ctx.lineTo(this.x + this.width + 20, this.y);
        ctx.fillStyle = '#8B4513';
        ctx.fill();

        // Окно
        ctx.fillStyle = '#ADD8E6';
        ctx.fillRect(this.x + 50, this.y + 30, 60, 50);

        // Дверь
        ctx.fillStyle = '#8B4513';
        ctx.fillRect(this.x + 130, this.y + 60, 40, 90);

        // Забор
        ctx.fillStyle = '#FFF';
        const fenceBaseY = this.y + this.height - 10;
        const fenceWidth = this.width + 100; // 300
        const startX = this.x - 60; // 240
        const boardWidth = 10;
        const boardSpacing = 20;

        for(let x = startX; x <= startX + fenceWidth; x += boardSpacing) {
            ctx.fillRect(x, fenceBaseY - 30, boardWidth, 40);
        }
        
        // Горизонтальные доски
        ctx.fillRect(startX, fenceBaseY -25, fenceWidth + boardWidth, boardWidth);
        ctx.fillRect(startX, fenceBaseY -5, fenceWidth + boardWidth, boardWidth);
    }
}

const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');
const house = new House();

function drawGround() {
    ctx.fillStyle = '#90EE90';
    ctx.fillRect(0, 450, canvas.width, 150);
}

function draw() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    drawGround();
    house.draw(ctx);
}

draw();