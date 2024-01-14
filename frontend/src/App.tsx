import {
  createBrowserRouter,
  createRoutesFromElements,
  Route,
} from "react-router-dom";

import Signin from './pages/Login.tsx';
import Signup from './pages/Signup.tsx';
import './styles.css';
import React from "react";

const App = createBrowserRouter(
  createRoutesFromElements(
    <>
      <Route path="/">
        <Route index element={<Signin />} />
        <Route path="login" element={<Signin />} />
        <Route path="register" element={<Signup />} />
      </Route>
    </>
  )
);
export default App