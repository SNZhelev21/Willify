from typing import Union
from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
import uvicorn
from endpoints.Users import users_router

app = FastAPI()

app.include_router(users_router)

origins = [
    "*"
]

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["GET, POST, PUT, DELETE"],
    allow_headers=["*"]
)

if __name__ == "__main__":
    # get local ipv4 address
    import socket
    localIp = socket.gethostbyname(socket.gethostname())
    print("\033[1;34m[*] local ip: ", localIp, "\033[0m")
    uvicorn.run(app, host=localIp, port=45098)