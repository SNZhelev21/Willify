import {Form, Link} from "react-router-dom";
import storageService from "../services/storage-service";
import userAuthApi from "../api/user-auth-api";
import { UserIM } from "../models";
import React from "react";
import {useNavigate} from "react-router-dom"

let userdata: UserIM = {
    first_name: "",
    last_name: "",
    username: "",
    email: "",
    password: ""
};

function Signup()
{
  let navigate = useNavigate();
    
  function onChangeEmail(e: React.FormEvent<HTMLInputElement>):void {
    userdata.email = e.currentTarget.value;
  };

  function onChangePassword(e: React.FormEvent<HTMLInputElement>):void {
    userdata.password = e.currentTarget.value;
  };

  function onChangeFirstName(e: React.FormEvent<HTMLInputElement>):void {
    userdata.first_name = e.currentTarget.value;
  };

  function onChangeLastName(e: React.FormEvent<HTMLInputElement>):void {
    userdata.last_name = e.currentTarget.value;
  };
  
  function onChangeUsername(e: React.FormEvent<HTMLInputElement>):void {
    userdata.username = e.currentTarget.value;
  };

  async function submit() {
    // api call for register
    userAuthApi.apiUserAuthRegisterPost(userdata).then(function (response) {
        storageService.saveAccessToken((response as any).data.accessToken);
        navigate("/home")
    }).catch(function (error) {
        alert("Error: " + error);
    });
  };
  
  return (
    <>
        <div className="grid w-full p-10 bg-gray-500 rounded-lg shadow">
            <h1 className="mb-5 text-xl font-bold leading-tight tracking-tight text-gray-900 md:text-2xl dark:text-white text-center">
                Регистрация
            </h1>
            <Form className="grid">
                <input
                    className="m-3 font-bold rounded text-slate-500"
                    type="text"
                    onChange={onChangeFirstName}
                    placeholder="Име"
                    required
                >
                </input>

                <input className="m-3 font-bold rounded text-slate-500"
                    type="text"
                    onChange={onChangeLastName}
                    placeholder="Фамилия"
                    required
                >
                </input>

                <input className="m-3 font-bold rounded text-slate-500"
                type="text"
                onChange={onChangeUsername}
                placeholder="Потребителско име"
                required
                >
                </input>

                <input
                    className="m-3 font-bold rounded text-slate-500"
                    type="email"
                    onChange={onChangeEmail}
                    placeholder="Имейл"
                    required
                >
                </input>

                <input
                    className="m-3 font-bold rounded text-slate-500"
                    type="password"
                    onChange={onChangePassword}
                    placeholder="Парола"
                    required
                >
                </input>
                
                <button id="submit" type="submit" onClick={submit} className="m-3 text-black transition-all duration-150 rounded hover:bg-slate-300 bg-slate-50 ">
                    Регистрация
                </button>

                <Link to='/login' className="text-sm text-gray-300 hover:text-gray-400 text-center">
                    Вече имате акаунт? Вход
                </Link>
            </Form>
        </div> 
    </>
  );
}

document.getElementById("submit")?.addEventListener("click", function(event) {
  event.preventDefault();
})

export default Signup;