import {Form, Link, NavLink, Outlet} from "react-router-dom";
import storageService from "../services/storage-service";
import userAuthApi from "../api/user-auth-api";
import { UserLM } from "../models";
import React from "react";
import {useNavigate} from "react-router-dom"

function Home() {
<<<<<<< Updated upstream
    return (
        <>
            <div className="w-screen h-screen">
                <nav className="w-screen h-[80px] bg-neutral-700 justify-center items-center gap-[200px] inline-flex">
                    <NavLink to="/stocks" className="w-fit text-center text-white aria-[current=page]:text-primary hover:text-primary transition ease-in-out duration-100">АКТИВИ</NavLink>
                    <NavLink to="/home" className="w-fit text-center text-white aria-[current=page]:text-primary hover:text-primary transition ease-in-out duration-100">НАЧАЛО</NavLink>
                    <NavLink to="/wills" className="w-fit text-center text-white aria-[current=page]:text-primary hover:text-primary transition ease-in-out duration-100">ЗАВЕЩАНИЯ</NavLink>
                </nav>
=======
    const [user, setUser] = useState<UserVM | null>(null);
    let navigate = useNavigate();

    useEffect(() => {
        (async () => {
            const userInfo: UserVM | null = storageService.retrieveUserInfo();
            tokenApi.apiVerifyToken().then(function(tokenResponse) {
                if (tokenResponse.status !== 200) {
                    navigate("/");
                    return;
                }
                
                if (userInfo) {
                    setUser(userInfo);
                    return;
                }
                
                userApi.apiUserGet().then(function(response) {
                    setUser(response.data as unknown as UserVM | null);
                    storageService.saveUserInfo(response.data as unknown as UserVM | null);
                });           
            }).catch(function() {
                navigate("/");
            });
        })();
    }, []);

    return (
        <>
            <div className="relative top-0 left-0 w-screen h-[100%] min-h-[100%]">
                <div className="sticky top-0 left-0 bg-white w-screen p-3">
                    <nav className="w-[62%] mx-auto grid grid-cols-3 grid-rows-1 place-items-center">
                        <NavLink to="/assets" className="text-3xl text-black hover:text-primary hover:text-4xl transition-all duration-150 ease-in-out aria-[current=page]:text-4xl aria-[current=page]:text-primary aria-[current=page]:bg-transparent">Assets</NavLink>
                        <NavLink to="/home" className="text-3xl text-black hover:text-primary hover:text-4xl transition-all duration-150 ease-in-out aria-[current=page]:text-4xl aria-[current=page]:text-primary aria-[current=page]:bg-transparent">Home</NavLink>
                        <NavLink to="/wills" className="text-3xl text-black hover:text-primary hover:text-4xl transition-all duration-150 ease-in-out aria-[current=page]:text-4xl aria-[current=page]:text-primary aria-[current=page]:bg-transparent">Wills</NavLink>
                    </nav>
                </div>

                <div className="w-screen h-[1900px] bg-[#ECECEC]"></div>
>>>>>>> Stashed changes
            </div>

            <Outlet/>
        </>
    ); 
}

export default Home;