import { useNavigate } from "react-router-dom";
import { setMapBuffer } from "../wasm/mapStore";
import { useRef, useState } from "react";
import { MapEditor, type MapEditorHandle } from "../components/MapEditor";

export function Config() {
  const [mapHeight, setMapHeight] = useState<string | null>(null);
  const [mapWidth, setMapWidth] = useState<string | null>(null);
  const [showEditor, setShowEditor] = useState(false);
  const editorRef = useRef<MapEditorHandle>(null);
  const navigate = useNavigate();
  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    if (mapHeight && mapWidth) setShowEditor(true);
  }
  const handleClick = () => {
    const map = (editorRef.current?.getMap());
    if (!map) return;
    setMapBuffer(map, Number(mapWidth), Number(mapHeight));
    navigate("render");
  }

  return (
    <section className="h-full">
      <div className="h-full w-full flex justify-around  items-center">
        <div className="p-6 rounded-lg border border-white/10 pb-12 flex flex-col gap-5">
          <form className="flex flex-col gap-5" onSubmit={handleSubmit}>
            <input type="text" placeholder="Enter Width" className="border" onChange={(e) => { setMapWidth(e.target.value) }} />
            <input type="text" placeholder="Enter Height" className="border" onChange={(e) => { setMapHeight(e.target.value) }} />
            <button className="buttonStyle" type="submit">Confirm</button>
          </form>
          {showEditor &&
          <div className="flex flex-col gap-5">
            <MapEditor ref={editorRef} mapHeight={Number(mapHeight)} mapWidth={Number(mapWidth)} />
            <button className="buttonStyle" onClick={handleClick}>Render</button>
          </div>
          }

        </div>
      </div>
    </section>
  )
}
