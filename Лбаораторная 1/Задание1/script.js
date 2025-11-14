document.addEventListener("DOMContentLoaded", () => {
    const container = document.querySelector(".initials-container");

    function createLetter(elements, color, id) {
        const svg = document.createElementNS("http://www.w3.org/2000/svg", "svg");
        svg.setAttribute("class", "letter");
        svg.setAttribute("id", id);
        svg.setAttribute("viewBox", "0 0 100 150");
        svg.setAttribute("width", "100");
        svg.setAttribute("height", "150");
        
        elements.forEach(({x, y, width, height}) => {
            const rect = document.createElementNS("http://www.w3.org/2000/svg", "rect");
            rect.setAttribute("x", x);
            rect.setAttribute("y", y);
            rect.setAttribute("width", width);
            rect.setAttribute("height", height);
            rect.setAttribute("fill", color);
            svg.appendChild(rect);
        });
        
        return svg;
    }

    const lettersConfig = [
        {   //Г
            elements: [
                {x:10, y:10, width:80, height:20},
                {x:10, y:10, width:20, height:130}
            ],
            color: "blue",
            id: "letter-g"
        },
        {   //Р
            elements: [
                {x:10, y:10, width:20, height:130},
                {x:10, y:10, width:80, height:20},
                {x:70, y:10, width:20, height:60},
                {x:10, y:60, width:80, height:20}
            ],
            color: "red",
            id: "letter-r1"
        },
        {   //Р
            elements: [
                {x:10, y:10, width:20, height:130},
                {x:10, y:10, width:80, height:20},
                {x:70, y:10, width:20, height:60},
                {x:10, y:60, width:80, height:20}
            ],
            color: "green",
            id: "letter-r2"
        }
    ];

    lettersConfig.forEach(config => {
        container.appendChild(createLetter(config.elements, config.color, config.id));
    });

    document.querySelectorAll(".letter").forEach((letter, index) => {
        setTimeout(() => {
            setInterval(() => {
                letter.style.transform = "translateY(-30px)";
                setTimeout(() => letter.style.transform = "translateY(0)", 300);
            }, 600);
        }, index * 200);
    });
});