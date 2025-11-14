document.addEventListener('DOMContentLoaded', function() {
    const canvas = document.getElementById('drawingCanvas');
    const ctx = canvas.getContext('2d');
    const newBtn = document.getElementById('newBtn');
    const openBtn = document.getElementById('openBtn');
    const saveBtn = document.getElementById('saveBtn');
    const fileInput = document.getElementById('fileInput');
    
//TODO не обрывать рисование при выходе за экран

    let isDrawing = false;
    let lastX = 0;
    let lastY = 0;
    const brushSize = 3;
    const brushColor = '#000000';
    
    function initCanvas() {
        ctx.fillStyle = 'white';
        ctx.fillRect(0, 0, canvas.width, canvas.height);
        ctx.strokeStyle = brushColor;
        ctx.lineWidth = brushSize;
    }
    
    function isLeftButtonPressed(event) {
        return event.buttons === 1;
    }
    
    canvas.addEventListener('mousedown', function(e) {
        if (e.button === 0) {
            isDrawing = true;
            [lastX, lastY] = [e.offsetX, e.offsetY];
        }
    });
    
    canvas.addEventListener('mousemove', function(e) {
        if (!isDrawing || !isLeftButtonPressed(e)) return;
        
        ctx.beginPath();
        ctx.moveTo(lastX, lastY);
        ctx.lineTo(e.offsetX, e.offsetY);
        ctx.stroke();
        
        [lastX, lastY] = [e.offsetX, e.offsetY];
    });
    
    canvas.addEventListener('mouseup', function(e) {
        if (e.button === 0) {
            isDrawing = false;
        }
    });
    
    canvas.addEventListener('mouseout', function() {
        isDrawing = false;
    });
    
    newBtn.addEventListener('click', function() {
        initCanvas();
    });
    
    openBtn.addEventListener('click', function() {
        fileInput.click();
    });
    
    fileInput.addEventListener('change', function(e) {
        const file = e.target.files[0];
        if (!file) return;
        
        const reader = new FileReader();
        reader.onload = function(event) {
            const img = new Image();
            img.onload = function() {
                canvas.width = img.width;
                canvas.height = img.height;
                ctx.drawImage(img, 0, 0);
            };
            img.src = event.target.result;
        };
        reader.readAsDataURL(file);
    });
    
    saveBtn.addEventListener('click', function() {
        const link = document.createElement('a');
        let fileName = 'drawing';
        
        link.download = `${fileName}.png`;
        link.href = canvas.toDataURL('image/png');
        link.click();
    });
    
    initCanvas();
});