import { useEffect, useRef } from "react";
import { createEngineRuntime } from "../wasm/engine";
import { mapBuffer, mapWidth, mapHeight } from "../wasm/mapStore";

export function Render() {
  const screenRef = useRef<HTMLCanvasElement>(null);
  const mapRef = useRef<HTMLCanvasElement>(null);
  const engineRef = useRef<any>(null);
  const touchStart = useRef<{ x: number; y: number } | null>(null);
  const lastTouch = useRef<{ x: number; y: number } | null>(null);
  function onTouchStart(e: TouchEvent) {
    if (e.touches.length !== 1) return;

    const t = e.touches[0];
    touchStart.current = { x: t.clientX, y: t.clientY };
    lastTouch.current = { x: t.clientX, y: t.clientY };
  }

  function onTouchMove(e: TouchEvent) {
    const engine = engineRef.current;
    if (!engine || !touchStart.current || e.touches.length !== 1) return;

    const t = e.touches[0];
    const dx = t.clientX - lastTouch.current!.x;
    const dy = t.clientY - lastTouch.current!.y;

    lastTouch.current = { x: t.clientX, y: t.clientY };

    // tweak these to taste
    const moveSpeed = 0.02;
    const rotateSpeed = 0.1;

    // vertical swipe → move
    engine.move(-dy * moveSpeed);

    // horizontal swipe → rotate
    engine.rotate(dx * rotateSpeed);
  }

  function onTouchEnd() {
    touchStart.current = null;
    lastTouch.current = null;
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
  return (
    <section className="h-full">
      <div className="h-full w-full flex flex-col justify-around  items-center">
        <canvas ref={screenRef} className="touch-none"/>
        <canvas ref={mapRef} />
      </div>
    </section>)
}
