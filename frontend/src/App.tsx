import {
  createBrowserRouter,
  createRoutesFromElements,
  Route,
} from "react-router-dom";

import Signin from './pages/Login.tsx';
import Signup from './pages/Signup.tsx';
import './styles.css';
import React from "react";
import Landing from './pages/Landing.tsx';

const App = createBrowserRouter(
  createRoutesFromElements(
    <>
      <Route path="/">
        <Route index element={<Landing />} />s
        <Route path="login" element={<Signin />} />
        <Route path="register" element={<Signup />} />
      </Route>
    </>
  )
);
export default App