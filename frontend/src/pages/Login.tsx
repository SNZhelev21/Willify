import {Form, Link, NavLink} from "react-router-dom";
import storageService from "../services/storage-service";
import userAuthApi from "../api/user-auth-api";
import { UserLM } from "../models";
import React from "react";
import {useNavigate} from "react-router-dom"

let userdata: UserLM = {
    username: "",
    password: ""
};

function Signin() {
    let navigate = useNavigate();

    function onChangeUsername(e: React.FormEvent<HTMLInputElement>): void {
        userdata.username = e.currentTarget.value;
    };
    
    function onChangePassword(e: React.FormEvent<HTMLInputElement>): void {
        userdata.password = e.currentTarget.value;
    };
    
    async function submit() {
        console.log(`Username: ${userdata.username} Password: ${userdata.password}`);
        await userAuthApi.apiUserAuthLoginPost(userdata).then(function (response) {
            storageService.saveAccessToken((response as any).data.accessToken);
            navigate("/home")
        }).catch(function (error) {
            alert("Error: " + error);
        });
    }
    
    return (
        <>
            <div className="grid">
            <NavLink to="/#" className="justify-self-center flex items-center">
                <img src="/src/assets/Willify.svg" className="w-32 h-20"></img>
            </NavLink>

                <div className="grid w-full p-10 bg-gray-500 rounded-lg shadow">
                    <Form className="grid">
                        <input 
                            className="m-3 font-bold rounded text-slate-500"
                            type="email" 
                            placeholder="Username" 
                            onChange={onChangeUsername} 
                        />
                        <input 
                            className="m-3 font-bold rounded text-slate-500"
                            type="password" 
                            placeholder="Password" 
                            onChange={onChangePassword} 
                        />
                        <button className="m-3 text-black transition-all duration-150 rounded hover:bg-slate-300 bg-slate-50" id="sumbit" type="submit" onClick={submit}>
                            Login
                        </button>
                        <Link to="/register" className="text-sm text-gray-300 transition-all duration-150 hover:text-gray-400 text-center">
                            Don't have an account? Sign up
                        </Link>
                    </Form>
                </div>
            </div>
        </>
    );
}

document.getElementById("submit")?.addEventListener("click", function(event) {
    event.preventDefault();
})
  
export default Signin;