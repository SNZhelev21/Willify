import {Form, Link, NavLink, Outlet} from "react-router-dom";
import storageService from "../services/storage-service";
import userAuthApi from "../api/user-auth-api";
import { UserLM } from "../models";
import React from "react";
import {useNavigate} from "react-router-dom"

function Home() {
    return (
        <>
            <div className="w-screen h-screen">
                <nav className="w-screen h-[80px] bg-neutral-700 justify-center items-center gap-[200px] inline-flex">
                    <NavLink to="/stocks" className="w-fit text-center text-white aria-[current=page]:text-primary hover:text-primary transition ease-in-out duration-100">АКТИВИ</NavLink>
                    <NavLink to="/home" className="w-fit text-center text-white aria-[current=page]:text-primary hover:text-primary transition ease-in-out duration-100">НАЧАЛО</NavLink>
                    <NavLink to="/wills" className="w-fit text-center text-white aria-[current=page]:text-primary hover:text-primary transition ease-in-out duration-100">ЗАВЕЩАНИЯ</NavLink>
                </nav>
            </div>

            <Outlet/>
        </>
    ); 
}

export default Home;