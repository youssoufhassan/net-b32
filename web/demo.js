// demo.js
Module.onRuntimeInitialized = async () =>{
    try {
        const loadedImages = await loadImages();
        images.push(...loadedImages);

        start();
        resizeCanvas();
    } catch (error) {
        console.error("Erreur de chargement des images :", error);
    } 
};

const EMPTY = 0;      // empty shape
const ENDPOINT = 1;   // endpoint shape
const SEGMENT = 2;    // segment shape
const CORNER = 3;     // corner shape
const TEE = 4;        // tee shape
const CROSS = 5;      // cross shape
const NB_SHAPES = 6;  // nb of shapes

const NORTH = 0;      // north
const EAST = 1;       // east
const SOUTH = 2;      // south
const WEST = 3;       // west 
const NB_DIRS = 4;    // nb of directions

var cellSize = 160;

const images = [];

const imageSources = [
    "./images/empty.png",
    "./images/endpoint.png",
    "./images/segment.png",
    "./images/corner.png",
    "./images/tee.png",
    "./images/cross.png",
];
/* -------- variables -------- */
var g = undefined;
var game = document.getElementById('game');
var restart = document.getElementById('restart');
var random = document.getElementById("randomize");
var shuffle = document.getElementById("shuffle");
var undo = document.getElementById("undo");
var redo = document.getElementById("redo");
var solve = document.getElementById("solve");
        
/* -------- events -------- */
// restart button
restart.addEventListener('click', start);
// random button
random.addEventListener('click', function(){
    g = Module._new_random(5,5,0,0,0);
    Module._shuffle(g);
    printGame(g);
});
// shuffle button
shuffle.addEventListener('click', function(){
    Module._shuffle(g);
    printGame(g);
});
// undo button
undo.addEventListener('click', function(){
    Module._undo(g);
    printGame(g);
});
// redo button
redo.addEventListener('click', function(){
    Module._redo(g);
    printGame(g);
});
// solve button
solve.addEventListener('click', function() {
    Module._solve(g);
    printGame(g);
});
// game play move clicks
game.addEventListener('click', leftClick);
game.addEventListener('contextmenu', rightClick);
// resize window
window.addEventListener('resize', resizeCanvas, false);

/* -------- functions -------- */
// print function
function getFontSize(width, height, nb_cols, nb_rows) {
    if (nb_cols > nb_rows) { return width/nb_cols; }
    return height/nb_rows;
}

function printGame(g) {
    var ctx = game.getContext('2d');
    var width = game.width;
    var height = game.height;
    var nb_rows = Module._nb_rows(g);
    var nb_cols = Module._nb_cols(g);
    cellSize = width / nb_cols;
    ctx.clearRect(0,0,width,height);
    for (var row = 0; row < nb_rows; row++) {
        for (var col = 0; col < nb_cols; col++) {
            var s = Module._get_piece_shape(g, row, col);
            var o = Module._get_piece_orientation(g, row, col);
            var img;
            var angle = 0;
            if (s == EMPTY){
                img = images[0];
            } 
            else if (s == ENDPOINT){
                img = images[1]; 
            }
            else if (s == SEGMENT){
                img = images[2]; 
            }
            else if (s == CORNER){
                img = images[3]; 
            }
            else if (s == TEE){
                img = images[4]; 
            }
            else if (s == CROSS){
                img = images[5]; 
            }

            if (o == EAST){
                angle = 90 * Math.PI / 180;
            } 
            else if (o == SOUTH){
                angle = 180 * Math.PI / 180;
            } 
            else if (o == WEST){
                angle = 270 * Math.PI / 180;
            } 
            const x = col * cellSize;
            const y = row * cellSize;

            ctx.save();

            ctx.translate(x + cellSize/2, y + cellSize/2);
            ctx.rotate(angle);

            ctx.drawImage(img, -cellSize/2, -cellSize/2, cellSize, cellSize);

            ctx.restore();
        }
    }
    if(Module._won(g)) {
        console.log("gg !");
        ctx.clearRect(0,0,width,height);
        ctx.font = 'bold 25px Arial';
        ctx.fillText("Congratulation ! You won !", width/2, height/2);
    }
}

function loadImages() {
    return Promise.all(imageSources.map(src => new Promise((resolve, reject) => {
        const img = new Image();
        img.onload = () => resolve(img);
        img.onerror = reject;
        img.src = src;
    })));
}

// play move functions
function leftClick(event) {
    event.preventDefault();
    var posX = event.offsetX;
    var posY = event.offsetY;
    var width = game.width;
    var height = game.height;
    var nb_rows = Module._nb_rows(g);
    var nb_cols = Module._nb_cols(g);
    var pieceSize = getFontSize(width, height, nb_cols, nb_rows);
    var i = posY/pieceSize;
    var j = posX/pieceSize;
    Module._play_move(g,i,j,1);
    printGame(g);
}

function rightClick(event) {
    event.preventDefault();
    var posX = event.offsetX;
    var posY = event.offsetY;
    var width = game.width;
    var height = game.height;
    var nb_rows = Module._nb_rows(g);
    var nb_cols = Module._nb_cols(g);
    var pieceSize = getFontSize(width, height, nb_cols, nb_rows);
    var i = posY/pieceSize;
    var j = posX/pieceSize;
    Module._play_move(g,i,j,(-1));
    printGame(g);
}

// init function
function start() {
    console.log("call start routine");
    g = Module._new_default();
    printGame(g);
}

function resizeCanvas() {
    if (!g) return;

    const nbRows = Module._nb_rows(g);
    const nbCols = Module._nb_cols(g);

    const targetCanvasSize = 800;

    game.width = targetCanvasSize;
    game.height = targetCanvasSize;

    game.style.width = targetCanvasSize + "px";
    game.style.height = targetCanvasSize + "px";

    cellSize = targetCanvasSize / nbCols;

    printGame(g);
} 
