export interface EngineRuntime {
  start(): void;
  stop(): void;
  initEngine(config: {px: number, py: number, pa: number}): void;
  setMap(map: Uint8Array, mapW: number, mapH: number): void;
  move(amount: number): void;
  rotate(angle: number): void;
}

export async function createEngineRuntime(
  screenCanvas: HTMLCanvasElement,
  mapCanvas: HTMLCanvasElement,
): Promise<EngineRuntime> {
  const wasm = await WebAssembly.instantiateStreaming(
    fetch("/engine.wasm"),
    {}
  );

  const {
    memory,
    init,
    engine_screenbuffer,
    engine_screen_height,
    engine_screen_width,
    engine_top_down_screenbuffer,
    engine_map_screen_height,
    engine_map_screen_width,
    engine_draw_map_bg,
    engine_map_buffer,
    engine_move,
    engine_rotate,
    engine_render,
  } = wasm.instance.exports as any;

  // screen canvas setup
  const screenW = engine_screen_width();
  const screenH = engine_screen_height();
  const fbPtr = engine_screenbuffer();

  const fbU8 = new Uint8ClampedArray(
    memory.buffer,
    fbPtr,
    screenW * screenH * 4
  );

  screenCanvas.width = screenW;
  screenCanvas.height = screenH;
  const ctxScreen = screenCanvas.getContext("2d")!;
  ctxScreen.imageSmoothingEnabled = false;
  const imageDataScreen = new ImageData(fbU8, screenW, screenH);

  // map canvas setup
  let ctxMap: CanvasRenderingContext2D | null = null;
  let imageDataMap: ImageData | null = null;

  function setupMapCanvas() {
    engine_draw_map_bg();

    const mapW = engine_map_screen_width();
    const mapH = engine_map_screen_height();

    const tdfbPtr = engine_top_down_screenbuffer();
    const tdfbU8 = new Uint8ClampedArray(
      memory.buffer,
      tdfbPtr,
      mapW * mapH * 4
    );

    mapCanvas.width = mapW;
    mapCanvas.height = mapH;

    ctxMap = mapCanvas.getContext("2d")!;
    imageDataMap = new ImageData(tdfbU8, mapW, mapH);
  }

  let running = false;
  let rafId = 0;
  let last = performance.now();
  let frames = 0;

  function loop(now: number) {
    if (!running) return;

    frames++;
    if (now - last >= 1000) {
      const fps = (frames * 1000) / (now - last);
      console.log(`FPS: ${fps.toFixed(1)}`);
      frames = 0;
      last = now;
    }

    engine_render();
    ctxScreen.putImageData(imageDataScreen, 0, 0);
    if(ctxMap && imageDataMap)
      ctxMap.putImageData(imageDataMap, 0, 0);

    rafId = requestAnimationFrame(loop);
  }

  return {
    start() {
      if (!running) {
        running = true;
        rafId = requestAnimationFrame(loop);
      }
    },

    stop() {
      running = false;
      cancelAnimationFrame(rafId);
    },

    initEngine(config) {
      init(config.px, config.py, config.pa);  // call Engine class constructor
    },

    setMap(map, mapW, mapH) {
      const mapPtr = engine_map_buffer(mapW, mapH);
      const wasmMap = new Uint8Array(
        memory.buffer,
        mapPtr,
        map.length
      );
      wasmMap.set(map);
      setupMapCanvas();
    },

    move(amount: number) {
      engine_move(amount);
    },

    rotate(angle: number) {
      engine_rotate(angle);
    },
  };
}
