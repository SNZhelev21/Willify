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
        await userAuthApi.apiUserAuthLoginPost(userdata).then(function (response) {

            console.log(response.data.token)
            storageService.saveAccessToken((response as any).data.token);
            navigate("/home")
        }).catch(function (error) {
            alert("Error: " + error);
        });
    }
    
    return (
        <>
            <div className="grid grid-cols-2 place-items-center w-screen h-screen">

                <div className="justify-self-center">
                    <img src="/src/assets/blob.svg" className="w-[50rem] h-[40rem]"></img>
                </div>

                <div className="grid w-[50%] max-w-[550px] p-10 rounded-lg">
                    <NavLink to="/#" className="justify-self-center flex items-center">
                        <img src="/src/assets/Willify.svg" className="w-32 h-20"></img>
                    </NavLink>
                    <Form className="grid">
                        <input 
                            className="m-3 font-bold rounded text-slate-500 bg-[#F4F4F4]"
                            type="email" 
                            placeholder="Username" 
                            onChange={onChangeUsername}
                        />
                        <input 
                            className="m-3 font-bold rounded text-slate-500 bg-[#F4F4F4]"
                            type="password" 
                            placeholder="Password" 
                            onChange={onChangePassword} 
                        />
                        <button className="m-3 text-primary transition-all duration-150 border border-primary rounded hover:bg-gray-100" id="sumbit" type="submit" onClick={submit}>
                            Login
                        </button>
                        <Link to="/register" className="text-sm text-primary transition-all duration-150 hover:text-[#d61115] text-center">
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