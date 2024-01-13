import React from 'react'
import { createBrowserRouter, RouterProvider} from 'react-router-dom'

export default function App() {
  const BrowserRouter = createBrowserRouter([
  ])

  return (
    <RouterProvider router={BrowserRouter}/>
  )
}