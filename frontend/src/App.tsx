import {
  createBrowserRouter,
  createRoutesFromElements,
  Route,
} from "react-router-dom";

import Signin from './pages/Login.tsx';
import './styles.css';
import React from "react";

const App = createBrowserRouter(
  createRoutesFromElements(
    <>
      <Route path="/">
        <Route index element={<Signin />} />
        <Route path="login" element={<Signin />} />
      </Route>
    </>
  )
);
export default App