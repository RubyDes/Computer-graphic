// Данные для игры
const wordsData = [
    { word: "ПРОГРАММИРОВАНИЕ", hint: "Процесс создания компьютерных программ" },
    { word: "КОМПЬЮТЕР", hint: "Электронное устройство для обработки данных" },
    { word: "АЛГОРИТМ", hint: "Последовательность действий для решения задачи" },
    { word: "ФУНКЦИЯ", hint: "Блок кода, выполняющий определенную задачу" },
    { word: "ПЕРЕМЕННАЯ", hint: "Именованная область памяти для хранения данных" },
    { word: "МАССИВ", hint: "Структура данных для хранения набора элементов" },
    { word: "ОБЪЕКТ", hint: "Экземпляр класса в объектно-ориентированном программировании" },
    { word: "ЦИКЛ", hint: "Конструкция для многократного выполнения блока кода" },
    { word: "УСЛОВИЕ", hint: "Конструкция для выполнения кода при определенных условиях" },
    { word: "СТРОКА", hint: "Тип данных, представляющий текст" },
    { word: "ЖИРАФ", hint: "Самое высокое сухопутное животное" },
    { word: "СОЛНЦЕ", hint: "Центральная звезда нашей солнечной системы" },
    { word: "БИБЛИОТЕКА", hint: "Место, где хранятся книги" },
    { word: "ПУТЕШЕСТВИЕ", hint: "Посещение разных мест и стран" },
    { word: "МУЗЫКА", hint: "Искусство звуков" }
];

// Элементы DOM
const gallowsCanvas = document.getElementById('gallowsCanvas');
const ctx = gallowsCanvas.getContext('2d');
const wordDisplay = document.getElementById('wordDisplay');
const hintElement = document.getElementById('hint');
const lettersElement = document.getElementById('letters');
const messageElement = document.getElementById('message');
const newGameBtn = document.getElementById('newGameBtn');
const toggleViewBtn = document.getElementById('toggleViewBtn');

// Состояние игры
let gameState = {
    currentWord: '',
    hint: '',
    guessedLetters: [],
    wrongLetters: [],
    maxAttempts: 6,
    attempts: 0,
    isGameOver: false,
    alternativeView: false
};

// Инициализация игры
function initGame() {
    // Выбираем случайное слово
    const randomIndex = Math.floor(Math.random() * wordsData.length);
    gameState.currentWord = wordsData[randomIndex].word;
    gameState.hint = wordsData[randomIndex].hint;
    gameState.guessedLetters = [];
    gameState.wrongLetters = [];
    gameState.attempts = 0;
    gameState.isGameOver = false;
    
    // Очищаем сообщение
    messageElement.textContent = '';
    messageElement.className = '';
    
    // Обновляем отображение
    updateWordDisplay();
    updateHint();
    createLetterButtons();
    drawGallows();
}

// Обновление отображения слова
function updateWordDisplay() {
    let display = '';
    for (let letter of gameState.currentWord) {
        if (gameState.guessedLetters.includes(letter)) {
            display += letter + ' ';
        } else {
            display += '_ ';
        }
    }
    wordDisplay.textContent = display;
    
    // Проверка на победу
    if (!display.includes('_')) {
        gameState.isGameOver = true;
        messageElement.textContent = 'Поздравляем! Вы выиграли!';
        messageElement.className = 'win';
    }
}

// Обновление подсказки
function updateHint() {
    hintElement.textContent = `Подсказка: ${gameState.hint}`;
}

// Создание кнопок букв
function createLetterButtons() {
    lettersElement.innerHTML = '';
    const alphabet = 'АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ';
    
    for (let letter of alphabet) {
        const letterElement = document.createElement('div');
        letterElement.className = 'letter';
        letterElement.textContent = letter;
        
        // Проверяем, была ли буква уже использована
        if (gameState.guessedLetters.includes(letter) || gameState.wrongLetters.includes(letter)) {
            if (gameState.guessedLetters.includes(letter)) {
                letterElement.classList.add('correct');
            } else {
                letterElement.classList.add('incorrect');
            }
        } else {
            letterElement.addEventListener('click', () => handleLetterClick(letter));
        }
        
        lettersElement.appendChild(letterElement);
    }
}

// Обработка клика по букве
function handleLetterClick(letter) {
    if (gameState.isGameOver) return;
    
    if (gameState.currentWord.includes(letter)) {
        // Правильная буква
        gameState.guessedLetters.push(letter);
    } else {
        // Неправильная буква
        gameState.wrongLetters.push(letter);
        gameState.attempts++;
        
        // Проверка на проигрыш
        if (gameState.attempts >= gameState.maxAttempts) {
            gameState.isGameOver = true;
            messageElement.textContent = `Игра окончена! Загаданное слово: ${gameState.currentWord}`;
            messageElement.className = 'lose';
        }
    }
    
    // Обновляем отображение
    updateWordDisplay();
    createLetterButtons();
    drawGallows();
}

// Рисование виселицы
function drawGallows() {
    ctx.clearRect(0, 0, gallowsCanvas.width, gallowsCanvas.height);
    
    // Настройки рисования
    ctx.lineWidth = 3;
    ctx.strokeStyle = '#333';
    ctx.lineCap = 'round';
    
    if (gameState.alternativeView) {
        drawAlternativeGallows();
    } else {
        drawStandardGallows();
    }
    
    // Отображение количества попыток
    drawAttemptsCounter();
}

// Стандартное представление виселицы
function drawStandardGallows() {
    // Основание
    ctx.beginPath();
    ctx.moveTo(50, 250);
    ctx.lineTo(150, 250);
    ctx.stroke();
    
    // Вертикальная стойка
    ctx.beginPath();
    ctx.moveTo(100, 250);
    ctx.lineTo(100, 50);
    ctx.stroke();
    
    // Верхняя перекладина
    ctx.beginPath();
    ctx.moveTo(100, 50);
    ctx.lineTo(200, 50);
    ctx.stroke();
    
    // Веревка
    ctx.beginPath();
    ctx.moveTo(200, 50);
    ctx.lineTo(200, 80);
    ctx.stroke();
    
    // Рисуем части тела в зависимости от количества ошибок
    if (gameState.attempts >= 1) {
        // Голова
        ctx.beginPath();
        ctx.arc(200, 100, 20, 0, Math.PI * 2);
        ctx.stroke();
    }
    
    if (gameState.attempts >= 2) {
        // Тело
        ctx.beginPath();
        ctx.moveTo(200, 120);
        ctx.lineTo(200, 180);
        ctx.stroke();
    }
    
    if (gameState.attempts >= 3) {
        // Левая рука
        ctx.beginPath();
        ctx.moveTo(200, 130);
        ctx.lineTo(170, 160);
        ctx.stroke();
    }
    
    if (gameState.attempts >= 4) {
        // Правая рука
        ctx.beginPath();
        ctx.moveTo(200, 130);
        ctx.lineTo(230, 160);
        ctx.stroke();
    }
    
    if (gameState.attempts >= 5) {
        // Левая нога
        ctx.beginPath();
        ctx.moveTo(200, 180);
        ctx.lineTo(170, 220);
        ctx.stroke();
    }
    
    if (gameState.attempts >= 6) {
        // Правая нога
        ctx.beginPath();
        ctx.moveTo(200, 180);
        ctx.lineTo(230, 220);
        ctx.stroke();
        
        // Глаза (крестики)
        ctx.beginPath();
        ctx.moveTo(195, 95);
        ctx.lineTo(205, 105);
        ctx.moveTo(205, 95);
        ctx.lineTo(195, 105);
        ctx.stroke();
    }
}

// Альтернативное представление виселицы
function drawAlternativeGallows() {
    // Основание
    ctx.beginPath();
    ctx.moveTo(50, 250);
    ctx.lineTo(250, 250);
    ctx.stroke();
    
    // Вертикальные стойки
    ctx.beginPath();
    ctx.moveTo(100, 250);
    ctx.lineTo(100, 50);
    ctx.stroke();
    
    ctx.beginPath();
    ctx.moveTo(200, 250);
    ctx.lineTo(200, 50);
    ctx.stroke();
    
    // Верхняя перекладина
    ctx.beginPath();
    ctx.moveTo(100, 50);
    ctx.lineTo(200, 50);
    ctx.stroke();
    
    // Средняя перекладина
    ctx.beginPath();
    ctx.moveTo(100, 150);
    ctx.lineTo(200, 150);
    ctx.stroke();
    
    // Веревка
    ctx.beginPath();
    ctx.moveTo(150, 50);
    ctx.lineTo(150, 80);
    ctx.stroke();
    
    // Рисуем части тела в зависимости от количества ошибок
    if (gameState.attempts >= 1) {
        // Голова
        ctx.beginPath();
        ctx.arc(150, 100, 20, 0, Math.PI * 2);
        ctx.stroke();
    }
    
    if (gameState.attempts >= 2) {
        // Тело
        ctx.beginPath();
        ctx.moveTo(150, 120);
        ctx.lineTo(150, 180);
        ctx.stroke();
    }
    
    if (gameState.attempts >= 3) {
        // Левая рука
        ctx.beginPath();
        ctx.moveTo(150, 130);
        ctx.lineTo(120, 160);
        ctx.stroke();
    }
    
    if (gameState.attempts >= 4) {
        // Правая рука
        ctx.beginPath();
        ctx.moveTo(150, 130);
        ctx.lineTo(180, 160);
        ctx.stroke();
    }
    
    if (gameState.attempts >= 5) {
        // Левая нога
        ctx.beginPath();
        ctx.moveTo(150, 180);
        ctx.lineTo(120, 220);
        ctx.stroke();
    }
    
    if (gameState.attempts >= 6) {
        // Правая нога
        ctx.beginPath();
        ctx.moveTo(150, 180);
        ctx.lineTo(180, 220);
        ctx.stroke();
        
        // Глаза (крестики)
        ctx.beginPath();
        ctx.moveTo(145, 95);
        ctx.lineTo(155, 105);
        ctx.moveTo(155, 95);
        ctx.lineTo(145, 105);
        ctx.stroke();
    }
}

// Отображение счетчика попыток
function drawAttemptsCounter() {
    ctx.font = '14px Arial';
    ctx.fillStyle = '#666';
    ctx.fillText(`Попытки: ${gameState.attempts}/${gameState.maxAttempts}`, 10, 20);
}

// Переключение вида
function toggleView() {
    gameState.alternativeView = !gameState.alternativeView;
    drawGallows();
}

// Обработчики событий
newGameBtn.addEventListener('click', initGame);
toggleViewBtn.addEventListener('click', toggleView);

// Запуск игры при загрузке
window.addEventListener('load', initGame);