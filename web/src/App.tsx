import { Outlet } from "react-router-dom";

function App() {
  return (
    <>
      <div className="h-[100dvh] flex flex-col items-center bg-gray-800">
        <main className="h-full w-full p-4">
          <div className='w-full h-full px-4 rounded-xl shadow-xl bg-gray-600'>
            <Outlet />
          </div>
        </main>
      </div>
    </>
  )
}

export default App;
