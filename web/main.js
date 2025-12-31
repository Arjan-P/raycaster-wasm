const wasm = await WebAssembly.instantiateStreaming(
  fetch("engine.wasm"),
  {}
);

const {
  memory,
  render,
  getFramebuffer,
  getWidth,
  getHeight,
  move,
  rotate,
  initChunks,
  updateChunks
} = wasm.instance.exports;

const width = getWidth();
const height = getHeight();
const fbPtr = getFramebuffer();
initChunks();

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


const canvas = document.getElementById("screen");
canvas.width = width;
canvas.height = height;
const ctx = canvas.getContext("2d");

const imageData = new ImageData(fbU8, width, height);

window.addEventListener("keydown", e => {
  if (e.key === "w") move(0.5);
  if (e.key === "s") move(-0.5);
  if (e.key === "a") rotate(-10);
  if (e.key === "d") rotate(10);
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
  updateChunks();
  render();
  ctx.putImageData(imageData, 0, 0);

  requestAnimationFrame(loop);
}

requestAnimationFrame(loop);
