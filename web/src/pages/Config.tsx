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
    if(mapHeight && mapWidth) setShowEditor(true);
  }
  const handleClick = () => {
    const map = (editorRef.current?.getMap());
    if(!map) return;
    setMapBuffer(map, Number(mapWidth), Number(mapHeight));
    navigate("render");
  }
  return (
    <div>
      <form onSubmit={handleSubmit}>
        <input type="text" placeholder="Enter Width" onChange={(e) => {setMapWidth(e.target.value)}} />
        <input type="text" placeholder="Enter Height" onChange={(e) => {setMapHeight(e.target.value)}} />
        <button type="submit">Confirm</button>
      </form>
      {showEditor && <MapEditor ref={editorRef} mapHeight={Number(mapHeight)} mapWidth={Number(mapWidth)} />}
      <button onClick={handleClick}>Render</button>
    </div>
  )
}
