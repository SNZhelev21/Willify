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
                    type="text"
                    onChange={onChangeFirstName}
                    placeholder="First name"
                    required
                >
                </input>

                <input className="m-3 font-bold rounded text-slate-500 bg-[#F4F4F4]"
                    type="text"
                    onChange={onChangeLastName}
                    placeholder="Last name"
                    required
                >
                </input>

                <input className="m-3 font-bold rounded text-slate-500 bg-[#F4F4F4]"
                type="text"
                onChange={onChangeUsername}
                placeholder="Username"
                required
                >
                </input>

                <input
                    className="m-3 font-bold rounded text-slate-500 bg-[#F4F4F4]"
                    type="email"
                    onChange={onChangeEmail}
                    placeholder="Email"
                    required
                >
                </input>

                <input
                    className="m-3 font-bold rounded text-slate-500 bg-[#F4F4F4]"
                    type="password"
                    onChange={onChangePassword}
                    placeholder="Password"
                    required
                >
                </input>
                
                <button id="submit" type="submit" onClick={submit} className="m-3 text-primary transition-all duration-150 border border-primary rounded hover:bg-gray-100">
                    Sign up
                </button>

                <Link to='/login' className="text-sm text-primary transition-all duration-150 hover:text-[#d61115] text-center">
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