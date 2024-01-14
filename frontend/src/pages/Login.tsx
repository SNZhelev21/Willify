import {Form, Link} from "react-router-dom";
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
            <div className="grid w-full p-10 bg-gray-500 rounded-lg shadow">
                <h1 className="mb-5 text-xl font-bold leading-tight tracking-tight text-gray-900 md:text-2xl dark:text-white text-center">
                    Вход
                </h1>
                <Form className="grid">
                    <input 
                        className="m-3 font-bold rounded text-slate-500"
                        type="email" 
                        placeholder="Потребителско име" 
                        onChange={onChangeUsername} 
                    />
                    <input 
                        className="m-3 font-bold rounded text-slate-500"
                        type="password" 
                        placeholder="Парола" 
                        onChange={onChangePassword} 
                    />
                    <button className="m-3 text-black transition-all duration-150 rounded hover:bg-slate-300 bg-slate-50" id="sumbit" type="submit" onClick={submit}>
                        Вход
                    </button>
                    <Link to="/signup" className="text-sm text-gray-300 transition-all duration-150 hover:text-gray-400 text-center">
                        Регистриране
                    </Link>
                </Form>
            </div>
        </>
    );
}

document.getElementById("submit")?.addEventListener("click", function(event) {
    event.preventDefault();
})
  
export default Signin;