import {Form, Link, NavLink} from "react-router-dom";
import React from "react";
import {useNavigate} from "react-router-dom"

function Landing()
{
    return(
        <>
            <div className="w-full h-full min-h-[100vh]">
                <div className="backdrop-blur-sm bg-white/50 sticky top-0 left-0 z-50 w-screen">
                    <nav className="sm:px-4 w-[95%] bg-transparent mx-auto pb-5 mb-[10vh] pt-5 gap-x-4 grid grid-cols-2 place-items-center">
                        <div className="flex justify-self-start gap-4 w-fit h-fit">
                            <Link to="/" className="flex items-center gap-2">
                                <img src="/src/assets/Logo.svg" className="w-16 h-16"></img>
                                <h1 className="text-3xl font-bold text-primary">Willify</h1>
                            </Link>
                        </div>
                        <div className="flex justify-self-end gap-4">
                            <NavLink to="/login" className="p-2 px-4 bg-white border border-primary hover:bg-gray-100 text-primary rounded-md transition ease-in-out duration-100 h-fit">Login</NavLink>
                            <NavLink to="/register" className="p-2 px-4 bg-primary border border-primary rounded-md text-white h-fit">Signup</NavLink>
                        </div>
                    </nav>
                </div>

                <div className="w-screen">
                    <div className="sm:px-4 relative mx-auto grid grid-cols-2 place-items-center w-[62%] gap-20 pb-[80px]">
                        <div className="grid justify-self-end">
                            <div className="justify-self-end">
                                <h2 className="text-6xl text-black">Welcome to </h2>
                                <h1 className="text-6xl font-bold bg-gradient-to-r from-primary to-black bg-clip-text text-transparent pb-2 w-fit">Willify</h1>
                                <p className="text-xl font-normal text-black-500">Willify is your trusted partner for safeguarding your assets.
                                Providing a seamless and secure solution, Willify ensures that your efforts are entrusted to the right hands.
                                We provide a simple and easy to use platform to create and manage your wills.
                                </p>
                            </div>
                        </div>
                        <div className="justify-self-center">
                            <img src="/src/assets/blob.svg" className="w-[50rem] h-[40rem]"></img>
                        </div>
                    </div>
                </div>

                <div className="absolute left-0 top-[100%] bg-black w-screen h-fit">
                    <div className="mb-10 h-1"></div>
                    <div className="relative left-[19%] w-[62%] grid p-2">
                        <img src="/src/assets/Willify.svg" className="justify-self-start"></img>

                        <p className="text-white text-xl font-normal justify-self-center">© 2024 All rights reserved. Created by Willify</p>
                    </div>
                </div>
            </div>
        </>
    )
}

export default Landing