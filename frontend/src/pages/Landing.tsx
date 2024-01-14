import {Form, Link} from "react-router-dom";
import React from "react";
import {useNavigate} from "react-router-dom"

function Landing()
{
    return(
        <>
        <div className="grid w-screen">


 
        <nav style={{ 
      display: 'flex', 
      justifyContent: 'flex-end', 
      backgroundColor: 'white',
      padding: '15px',
      position: 'fixed',
      top: 0,
      left: 0,
      right: 0,
    }}>
      
                    <a style={{ color: 'black', textDecoration: 'none', padding: '10px 25px', margin: '0 20px', border: '2px solid red', borderRadius: '5px'}} 
                    href="#">Вход</a>
                    <a style={{ color: 'white', textDecoration: 'none', padding: '10px 25px', margin: '0 20px', border: '2px solid red', borderRadius: '5px', backgroundColor: 'red' }} href="#">Регистрация</a>
            </nav>
            <p className="text-red-400 text-center">
                рандъм текст
            </p>
        </div>
        </>
    )
}


      
 

export default Landing