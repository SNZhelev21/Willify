import {Form, Link, NavLink} from "react-router-dom";
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
        <div className="grid">
            <NavLink to="/#" className="justify-self-center flex items-center">
                <img src="/src/assets/Willify.svg" className="w-32 h-20"></img>
            </NavLink>

            <div className="grid w-full p-10 bg-gray-500 rounded-lg shadow">
                <Form className="grid">
                    <input
                        className="m-3 font-bold rounded text-slate-500"
                        type="text"
                        onChange={onChangeFirstName}
                        placeholder="First name"
                        required
                    >
                    </input>

                    <input className="m-3 font-bold rounded text-slate-500"
                        type="text"
                        onChange={onChangeLastName}
                        placeholder="Last name"
                        required
                    >
                    </input>

                    <input className="m-3 font-bold rounded text-slate-500"
                    type="text"
                    onChange={onChangeUsername}
                    placeholder="Username"
                    required
                    >
                    </input>

                    <input
                        className="m-3 font-bold rounded text-slate-500"
                        type="email"
                        onChange={onChangeEmail}
                        placeholder="Email"
                        required
                    >
                    </input>

                    <input
                        className="m-3 font-bold rounded text-slate-500"
                        type="password"
                        onChange={onChangePassword}
                        placeholder="Password"
                        required
                    >
                    </input>
                    
                    <button id="submit" type="submit" onClick={submit} className="m-3 text-black transition-all duration-150 rounded hover:bg-slate-300 bg-slate-50 ">
                        Sign up
                    </button>

                    <Link to='/login' className="text-sm text-gray-300 hover:text-gray-400 text-center">
                        Already have an account? Login
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

export default Signup;