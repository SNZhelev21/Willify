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
<<<<<<< Updated upstream
=======
import Home from './pages/Home.tsx';
import Assets from './pages/Assets.tsx';
import Wills from './pages/Wills.tsx';
>>>>>>> Stashed changes

const App = createBrowserRouter(
  createRoutesFromElements(
    <>
      <Route path="/">
        <Route index element={<Landing />} />s
        <Route path="login" element={<Signin />} />
        <Route path="register" element={<Signup />} />
<<<<<<< Updated upstream
=======
        <Route path="home" element={<Home />} />
        <Route path="assets" element={<Assets />} />
        <Route path="wills" element={<Wills />} />
>>>>>>> Stashed changes
      </Route>
    </>
  )
);
export default App