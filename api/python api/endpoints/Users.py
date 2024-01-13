import datetime
from fastapi import APIRouter, Depends, HTTPException, status
from fastapi.responses import HTMLResponse, JSONResponse, RedirectResponse, Response
from pydantic import BaseModel, EmailStr
import typing
import modules.database as db
import re
import hashlib
from dotenv import load_dotenv
from jose import jwt
import os

users_router: APIRouter = APIRouter()

config = load_dotenv(".env")

class Register(BaseModel):
    username: str
    first_name: str
    last_name: str
    password: str
    email: EmailStr
    
class Login(BaseModel):
    username: str
    password: str

@users_router.post("/register")
def register(user: Register):
    match: typing.Match | None = re.search(r'\W+', user.username)
    
    if match:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="Username must be alphanumeric")
    
    match = re.search(r'\W+', user.first_name)
    
    if match:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="First name must be alphanumeric")
    
    match = re.search(r'\W+', user.last_name)
    
    if match:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="Last name must be alphanumeric")
    
    match = re.search(r'\W+', user.password)
    
    if match:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="Password must only contain letters, numbers, !, #, $, %, ^, & and *")
    
    db.cur.execute("SELECT * FROM users WHERE username = %s", (user.username,))
    user_exists = db.cur.fetchone()
    
    if (user_exists):
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="Username already exists")
    
    db.cur.execute("SELECT * FROM users WHERE email = %s", (user.email,))
    user_exists = db.cur.fetchone()
    
    if (user_exists):
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="Email already exists")
    
    db.cur.execute("SELECT last_value FROM users_id_seq")
    id: int = int(db.cur.fetchone()["last_value"]) # type: ignore
    
    id = (id + 1) * 52834
    
    password_salt: str = user.email + user.username + user.password + str(id) + os.getenv("SECRET") # type: ignore
    
    password_hash: str = hashlib.sha512(password_salt.encode()).hexdigest()
    
    db.cur.execute("INSERT INTO users (id, username, first_name, last_name, password, email) VALUES (DEFAULT, %s, %s, %s, %s, %s)", (user.username, user.first_name, user.last_name, password_hash, user.email))
    db.conn.commit()
    return Response(status_code=status.HTTP_201_CREATED)
    
    
@users_router.post("/login", tags=["users"])
def login(user: Login):
    db.cur.execute("SELECT * FROM users WHERE username = %s", (user.username,))
    user_info = db.cur.fetchone()
    
    if not user_info:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail=f"User with username {user.username} not found")
    
    id: int = int(user_info["id"])
    password_db: str = user_info["password"]
    email: str = user_info["email"]
    first_name: str = user_info["first_name"]
    last_name: str = user_info["last_name"]
    role: str = user_info["role"]
    
    print(email, user.username, user.password, id)
    
    hasher = hashlib.sha512()
    hasher.update((email + user.username + user.password + str(id * 52834) + os.getenv("SECRET")).encode()) # type: ignore
    user.password = hasher.hexdigest()
    
    print(user.password, password_db)
    
    if user.password != password_db:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="Incorrect password")
    
    token = jwt.encode({
        "id": str(id),
        "username": user.username,
        "email": email,
        "first_name": first_name,
        "last_name": last_name,
        "role": role,
        "exp": datetime.datetime.utcnow() + datetime.timedelta(minutes=60),
        "iat": datetime.datetime.utcnow(),
        "iss": "auth0"
    }, key=os.getenv("RSA_SECRET"), algorithm="RS512") # type: ignore
    
    return Response(status_code=status.HTTP_200_OK, content=token)

@users_router.get("/user", tags=["Users"])
def get_user(token: str):
    payload: str = ""
    try:
        payload = jwt.decode(token, key=os.getenv("RSA_SECRET"), algorithms=["RS512"]) # type: ignore
    except Exception as e:
        raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail=f"{e}")
    
    print(payload)