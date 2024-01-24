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
import Home from './pages/Home.tsx';
import Home from './pages/Home.tsx';
import Assets from './pages/Assets.tsx';
import Wills from './pages/Wills.tsx';

const App = createBrowserRouter(
  createRoutesFromElements(
    <>
      <Route path="/">
        <Route index element={<Landing />} />s
        <Route path="login" element={<Signin />} />
        <Route path="register" element={<Signup />} />
        <Route path="home" element={<Home />} />
        <Route path="home" element={<Home />} />
        <Route path="assets" element={<Assets />} />
        <Route path="wills" element={<Wills />} />
      </Route>
    </>
  )
);
export default App