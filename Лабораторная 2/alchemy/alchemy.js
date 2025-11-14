const elements = {
    'earth': { name: 'Земля', icon: '🌍' },
    'fire': { name: 'Огонь', icon: '🔥' },
    'water': { name: 'Вода', icon: '💧' },
    'air': { name: 'Воздух', icon: '💨' },
    'steam': { name: 'Пар', icon: '☁️' },
    'lava': { name: 'Лава', icon: '🌋' },
    'dust': { name: 'Пыль', icon: '💨' },
    'gunpowder': { name: 'Порох', icon: '⚫' },
    'explosion': { name: 'Взрыв', icon: '💥' },
    'smoke': { name: 'Дым', icon: '💨' },
    'energy': { name: 'Энергия', icon: '⚡' },
    'stone': { name: 'Камень', icon: '🪨' },
    'storm': { name: 'Буря', icon: '⛈️' },
    'metal': { name: 'Металл', icon: '🔩' },
    'electricity': { name: 'Электричество', icon: '⚡' },
    'hydrogen': { name: 'Водород', icon: 'H' },
    'oxygen': { name: 'Кислород', icon: 'O' },
    'ozone': { name: 'Озон', icon: 'O₃' },
    'mud': { name: 'Грязь', icon: '🟫' },
    'geyser': { name: 'Гейзер', icon: '🌊' },
    'boiler': { name: 'Паровой котел', icon: '♨️' },
    'pressure': { name: 'Давление', icon: '⬇️' },
    'volcano': { name: 'Вулкан', icon: '🌋' },
    'explosive_gas': { name: 'Гремучий газ', icon: '💥' },
    'swamp': { name: 'Болото', icon: '🐊' },
    'alcohol': { name: 'Спирт', icon: '🍷' },
    'molotov': { name: 'Коктейль Молотова', icon: '🔥' },
    'life': { name: 'Жизнь', icon: '🧬' },
    'bacteria': { name: 'Бактерии', icon: '🦠' },
    'vodka': { name: 'Водка', icon: '🍸' }
};

const combinations = {
    'fire-water': ['steam', 'alcohol'],
    'fire-earth': ['lava'],
    'air-earth': ['dust'],
    'fire-dust': ['gunpowder'],
    'gunpowder-fire': ['explosion', 'smoke'],
    'air-fire': ['energy'],
    'lava-water': ['steam', 'stone'],
    'air-energy': ['storm'],
    'fire-stone': ['metal'],
    'metal-energy': ['electricity'],
    'electricity-water': ['hydrogen', 'oxygen'],
    'electricity-oxygen': ['ozone'],
    'dust-water': ['mud'],
    'steam-earth': ['geyser'],
    'steam-metal': ['boiler'],
    'boiler-steam': ['pressure'],
    'lava-pressure': ['volcano'],
    'hydrogen-oxygen': ['explosive_gas'],
    'water-earth': ['swamp'],
    'alcohol-fire': ['molotov'],
    'swamp-energy': ['life'],
    'life-swamp': ['bacteria'],
    'alcohol-water': ['vodka']
};

let game = {
    discovered: ['earth', 'fire', 'water', 'air'],
    slots: [null, null],
    combinations: 0
};

const elementsGrid = document.getElementById('elements-grid');
const slot1 = document.getElementById('slot1');
const slot2 = document.getElementById('slot2');
const trashBin = document.getElementById('trash-bin');

function init() {
    updateElements();
    updateSlots();
    setupEventListeners();
}

// отдлеить логику от визуального представления

function updateElements() {
    elementsGrid.innerHTML = '';
    game.discovered.forEach(id => {
        const element = elements[id];
        const div = document.createElement('div');
        div.className = 'element';
        div.draggable = true;
        div.dataset.id = id;
        div.innerHTML = `
            <div class="element-icon">${element.icon}</div>
            <div class="element-name">${element.name}</div>
        `;
        elementsGrid.appendChild(div);
    });
}

function updateSlots() {
    updateSlot(slot1, 0);
    updateSlot(slot2, 1);
}

function updateSlot(slotElement, slotIndex) {
    const placeholder = slotElement.querySelector('.placeholder');
    
    if (game.slots[slotIndex]) {
        const element = elements[game.slots[slotIndex]];
        if (placeholder) placeholder.remove();
        
        let elementDiv = slotElement.querySelector('.element');
        if (!elementDiv) {
            elementDiv = document.createElement('div');
            elementDiv.className = 'element';
            elementDiv.draggable = true;
            elementDiv.dataset.id = game.slots[slotIndex];
            slotElement.appendChild(elementDiv);
        }
        
        elementDiv.innerHTML = `
            <div class="element-icon">${element.icon}</div>
            <div class="element-name">${element.name}</div>
        `;

    } else {
        const existingElement = slotElement.querySelector('.element');
        if (existingElement) existingElement.remove();
    }
}

// Комбинирование элементов
function combineElements() {
    if (!game.slots[0] || !game.slots[1]) return;
    
    const el1 = game.slots[0];
    const el2 = game.slots[1];
    const key1 = `${el1}-${el2}`;
    const key2 = `${el2}-${el1}`;

    const result = combinations[key1] || combinations[key2];
    
    if (result) {
        game.combinations++;
        
        let newElements = false;
        result.forEach(id => {
            if (!game.discovered.includes(id)) {
                game.discovered.push(id);
                newElements = true;
            }
        });
        
        game.slots = [null, null];
        updateElements();
        updateSlots();
        updateStats();
    }
}

function checkCombination() {
    if (game.slots[0] && game.slots[1]) {
        setTimeout(combineElements, 100);
    }
}

function setupEventListeners() {
    document.addEventListener('dragstart', (e) => {
        if (e.target.classList.contains('element')) {
            e.dataTransfer.setData('text/plain', e.target.dataset.id);
        }
    });

    // Обработчики для слотов
    [slot1, slot2].forEach((slot, index) => {
        slot.addEventListener('dragover', (e) => {
            e.preventDefault();
            slot.classList.add('highlight');
        });

        slot.addEventListener('dragleave', () => {
            slot.classList.remove('highlight');
        });

        slot.addEventListener('drop', (e) => {
            e.preventDefault();
            slot.classList.remove('highlight');
            
            const elementId = e.dataTransfer.getData('text/plain');
            game.slots[index] = elementId;
            updateSlots();
            
            checkCombination();
        });
    });

    // Корзина
    trashBin.addEventListener('dragover', (e) => {
        e.preventDefault();
        trashBin.classList.add('highlight');
    });

    trashBin.addEventListener('dragleave', () => {
        trashBin.classList.remove('highlight');
    });

    trashBin.addEventListener('drop', (e) => {
        e.preventDefault();
        trashBin.classList.remove('highlight');
        
        const elementId = e.dataTransfer.getData('text/plain');
        const slotIndex = game.slots.indexOf(elementId);
        
        if (slotIndex !== -1) {
            game.slots[slotIndex] = null;
            updateSlots();
        }
    });

    document.getElementById('sort-btn').addEventListener('click', () => {
        game.discovered.sort((a, b) => elements[a].name.localeCompare(elements[b].name));
        updateElements();
    });
}

document.addEventListener('DOMContentLoaded', init);