const canvas = document.getElementById("c");
const ctx = canvas.getContext("2d");
let last = performance.now();
let frames = 0;

const wasm = await WebAssembly.instantiateStreaming(
  fetch("engine.wasm"),
  {}
);

const {
  memory,
  render,
  get_columns,
  move,
  rotate
} = wasm.instance.exports;

window.addEventListener("keydown", e => {
  if (e.key === "w") move(0.5);
  if (e.key === "s") move(-0.5);
  if (e.key === "a") rotate(-10);
  if (e.key === "d") rotate(10);
});

function loop(now) {
  frames++;

  if (now - last >= 1000) {
    const fps = (frames * 1000) / (now - last);
    console.log(`FPS: ${fps.toFixed(1)}`);
    frames = 0;
    last = now;
  }
  render(canvas.width, canvas.height);

  const ptr = get_columns();
  const cols = new Int32Array(
    memory.buffer,
    ptr,
    canvas.width * 3
  );

  ctx.clearRect(0, 0, canvas.width, canvas.height);

  for (let x = 0; x < canvas.width; x++) {
    const ceiling = cols[x * 3];
    const floor = cols[x * 3 + 1];
    const shade = cols[x * 3 + 2]

    // shade sky
    ctx.fillStyle = "#000";
    ctx.fillRect(x, 0, 1, ceiling);

    // shade wall

    ctx.fillStyle = `rgb(${shade},${shade}, ${shade})`;
    ctx.fillRect(x, ceiling, 1, floor - ceiling);

    // shade ground
    ctx.fillStyle = "#400";
    ctx.fillRect(x, floor, 1, canvas.height - floor);
  }

  requestAnimationFrame(loop);
}

requestAnimationFrame(loop);
