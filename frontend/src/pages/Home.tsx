import {Form, Link, NavLink, Outlet} from "react-router-dom";
import storageService from "../services/storage-service";
import userApi from "../api/user-api";
import {UserVM} from "../models/user-vm";
import React, {useState, useEffect} from "react";
import {useNavigate} from "react-router-dom"

function Home() {
    const [user, setUser] = useState<UserVM | null>(null);

    useEffect(() => {
        const userInfo: UserVM | null = storageService.retrieveUserInfo();
        if (!userInfo) {
            (async () => {
                try {
                    const response = await userApi.apiUserGet();
                    setUser(response.data as unknown as UserVM | null);
                    storageService.saveUserInfo(response.data as unknown as UserVM | null)
                } catch (error) {
                    console.log(error);
                }
            })();
        }
        else {
            setUser(userInfo);
        }
    }, []);

    return (
        <>
            <div className="w-screen h-screen">

                {user && (
                    <>
                        <h1>{user.id}</h1>
                        <h1>{user.username}</h1>
                        <h1>{user.first_name}</h1>
                        <h1>{user.last_name}</h1>
                        <h1>{user.email}</h1>
                        <h1>{user.role}</h1>
                    </>
                )}

            </div>

            <Outlet/>
        </>
    ); 
}

export default Home;