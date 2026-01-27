import { useEffect, useRef } from "react";
import { createEngineRuntime } from "../wasm/engine";
import { mapBuffer, mapWidth, mapHeight } from "../wasm/mapStore";

export function Render() {
  const screenRef = useRef<HTMLCanvasElement>(null);
  const mapRef = useRef<HTMLCanvasElement>(null);
  const engineRef = useRef<any>(null);
  const HORIZ_DEADZONE = 12; // px
  const VERT_DEADZONE = 10; // px

  const horizAccum = useRef(0);
  const vertAccum = useRef(0);

  const touchStart = useRef<{ x: number; y: number } | null>(null);
  const lastTouch = useRef<{ x: number; y: number } | null>(null);

  function onTouchStart(e: TouchEvent) {
    if (e.touches.length !== 1) return;

    const t = e.touches[0];
    touchStart.current = { x: t.clientX, y: t.clientY };
    lastTouch.current = { x: t.clientX, y: t.clientY };

    horizAccum.current = 0;
    vertAccum.current = 0;
  }

  function onTouchMove(e: TouchEvent) {
    const engine = engineRef.current;
    if (!engine || !lastTouch.current || e.touches.length !== 1) return;

    const t = e.touches[0];
    const dx = t.clientX - lastTouch.current.x;
    const dy = t.clientY - lastTouch.current.y;

    lastTouch.current = { x: t.clientX, y: t.clientY };

    horizAccum.current += dx;
    vertAccum.current += dy;

    const moveSpeed = 0.03;
    const rotateSpeed = 1;

    /* ---- VERTICAL (move) ---- */
    if (Math.abs(vertAccum.current) >= VERT_DEADZONE) {
      const effectiveDy =
        vertAccum.current > 0
          ? vertAccum.current - VERT_DEADZONE
          : vertAccum.current + VERT_DEADZONE;

      engine.move(-effectiveDy * moveSpeed);
      vertAccum.current = 0;
    }

    /* ---- HORIZONTAL (rotate) ---- */
    if (Math.abs(horizAccum.current) >= HORIZ_DEADZONE) {
      const effectiveDx =
        horizAccum.current > 0
          ? horizAccum.current - HORIZ_DEADZONE
          : horizAccum.current + HORIZ_DEADZONE;

      engine.rotate(effectiveDx * rotateSpeed);
      horizAccum.current = 0;
    }
  }

  function onTouchEnd() {
    touchStart.current = null;
    lastTouch.current = null;
    horizAccum.current = 0;
    vertAccum.current = 0;
  }


  useEffect(() => {
    if (!screenRef.current || !mapRef.current) return;

    let alive = true;

    createEngineRuntime(screenRef.current, mapRef.current)
      .then(engine => {
        if (!alive) return;
        if (!mapBuffer || !mapHeight || !mapWidth) return;
        engineRef.current = engine;
        engine.initEngine({ px: 4, py: 4, pa: 0 });
        engine.setMap(mapBuffer, mapWidth, mapHeight);
        engine.start();
      });

    return () => {
      alive = false;
      engineRef.current?.stop();
    };
  }, []);

  useEffect(() => {
    const canvas = screenRef.current;
    if (!canvas) return;

    canvas.addEventListener("touchstart", onTouchStart, { passive: false });
    canvas.addEventListener("touchmove", onTouchMove, { passive: false });
    canvas.addEventListener("touchend", onTouchEnd);
    canvas.addEventListener("touchcancel", onTouchEnd);

    return () => {
      canvas.removeEventListener("touchstart", onTouchStart);
      canvas.removeEventListener("touchmove", onTouchMove);
      canvas.removeEventListener("touchend", onTouchEnd);
      canvas.removeEventListener("touchcancel", onTouchEnd);
    };
  }, []);

  useEffect(() => {
    function onKey(e: KeyboardEvent) {
      const engine = engineRef.current;
      if (!engine) return;

      if (e.key === "w") engine.move(0.5);
      if (e.key === "s") engine.move(-0.5);
      if (e.key === "a") engine.rotate(-10);
      if (e.key === "d") engine.rotate(10);
    }

    window.addEventListener("keydown", onKey);
    return () => window.removeEventListener("keydown", onKey);
  }, []);
  useEffect(() => {
    function onResize() {
      // CSS handles scale — no need to touch canvas width/height
    }

    window.addEventListener("resize", onResize);
    return () => window.removeEventListener("resize", onResize);
  }, []);
  return (
    <section className="h-full">
      <div className="h-full w-full flex flex-col justify-around  items-center">
        <canvas ref={screenRef} className="w-full h-full image-rendering-pixelated touch-none" />
        <div className="w-full h-full flex items-center justify-center">
          <canvas
            ref={screenRef}
            className="max-w-full max-h-full aspect-[4/3] touch-none"
          />
        </div>

        <div className="w-full h-full flex items-center justify-center">
          <canvas ref={mapRef} />
        </div>
      </div>
    </section>)
}
