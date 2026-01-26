export let mapBuffer: Uint8Array | null = null;
export let mapWidth: number | null = null;
export let mapHeight: number | null = null;

export function setMapBuffer(b: Uint8Array, w: number, h: number) {
  mapBuffer = b;
  mapWidth = w;
  mapHeight = h;
}
