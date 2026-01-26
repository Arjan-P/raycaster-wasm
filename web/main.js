const wasm = await WebAssembly.instantiateStreaming(
  fetch("./engine.wasm"),
  {}
);

const {
  memory,
  init,
  engine_map_init,
  engine_screenbuffer,
  engine_top_down_screenbuffer,
  engine_screen_height,
  engine_screen_width,
  engine_map_screen_height,
  engine_map_screen_width,
  engine_move,
  engine_rotate,
  engine_render,
} = wasm.instance.exports;
init();
engine_map_init();

const width = engine_screen_width();
const height = engine_screen_height();
const mapWidth = engine_map_screen_width();
const mapHeight = engine_map_screen_height();
const fbPtr = engine_screenbuffer();
const tdfbPtr = engine_top_down_screenbuffer();

const fbU32 = new Uint32Array(
  memory.buffer,
  fbPtr,
  width * height
);

const fbU8 = new Uint8ClampedArray(
  memory.buffer,
  fbPtr,
  width * height * 4
);

const tdfbU32 = new Uint32Array(
  memory.buffer,
  tdfbPtr,
  mapWidth * mapHeight
);

const tdfbU8 = new Uint8ClampedArray(
  memory.buffer,
  tdfbPtr,
  mapWidth * mapHeight * 4
);

const canvasScreen = document.getElementById("screen");
canvasScreen.width = width;
canvasScreen.height = height;
const ctxScreen = canvasScreen.getContext("2d");

const canvasMap = document.getElementById("map");
canvasMap.width = mapWidth;
canvasMap.height = mapHeight;
const ctxMap = canvasMap.getContext("2d");

const imageDataScreen = new ImageData(fbU8, width, height);
const imageDataMap = new ImageData(tdfbU8, mapWidth, mapHeight);

window.addEventListener("keydown", e => {
  if (e.key === "w") engine_move((1 << 14) * 0.5);
  if (e.key === "s") engine_move(-(1 << 14) * 0.5);
  if (e.key === "a") engine_rotate(-10);
  if (e.key === "d") engine_rotate(10);
});

let last = performance.now();
function loop(now) {
  frames++;

  if (now - last >= 1000) {
    const fps = (frames * 1000) / (now - last);
    console.log(`FPS: ${fps.toFixed(1)}`);
    frames = 0;
    last = now;
  }
  engine_render();
  ctxScreen.putImageData(imageDataScreen, 0, 0);
  ctxMap.putImageData(imageDataMap, 0, 0);

  requestAnimationFrame(loop);
}

requestAnimationFrame(loop);
