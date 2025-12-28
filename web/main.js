const canvas = document.getElementById("c");
const ctx = canvas.getContext("2d");

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
  if (e.key === "w") move(0.2);
  if (e.key === "s") move(-0.2);
  if (e.key === "a") rotate(-0.1);
  if (e.key === "d") rotate(0.1);
});

function loop() {
  render(canvas.width, canvas.height);

  const ptr = get_columns();
  const cols = new Int32Array(
    memory.buffer,
    ptr,
    canvas.width * 2
  );

  ctx.clearRect(0, 0, canvas.width, canvas.height);

  for (let x = 0; x < canvas.width; x++) {
    const ceiling = cols[x * 2];
    const floor   = cols[x * 2 + 1];

    ctx.fillStyle = "#000";
    ctx.fillRect(x, 0, 1, ceiling);

    ctx.fillStyle = "#aaa";
    ctx.fillRect(x, ceiling, 1, floor - ceiling);

    ctx.fillStyle = "#444";
    ctx.fillRect(x, floor, 1, canvas.height - floor);
  }

  requestAnimationFrame(loop);
}

loop();
