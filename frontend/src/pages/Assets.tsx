import {Form, Link, NavLink, Outlet} from "react-router-dom";
import storageService from "../services/storage-service";
import userApi from "../api/user-api";
import tokenApi from "../api/token-api";
import assetsApi from "../api/assets-api";
import {UserVM} from "../models/user-vm";
import {AssetIM} from "../models/asset-im";
import {AssetVM} from "../models/asset-vm";
import React, {useState, useEffect} from "react";
import {useNavigate} from "react-router-dom";
import Modal from 'react-modal';

Modal.setAppElement('body');

let assetData: AssetIM = {
    name: "",
    type: "",
    quantity: 0
};

function Assets() {
    const [user, setUser] = useState<UserVM | null>(null);
    const [assets, setAssets] = useState<AssetVM[] | null>(null);
    const [modalOpen, setModalOpen] = useState<boolean>(false);
    let navigate = useNavigate();

    useEffect(() => {
        const userInfo: UserVM | null = storageService.retrieveUserInfo();
        (async () => {
            tokenApi.apiVerifyToken().then(function(tokenResponse) {
                if (tokenResponse.status !== 200) {
                    navigate("/");
                    return;
                }
                
                assetsApi.apiAssetsGetAll().then(function(response) {
                    setAssets(response.data as unknown as AssetVM[] | null);

                    console.log(assets);
                });

                if (userInfo) {
                    setUser(userInfo);
                    return;
                }
                
                userApi.apiUserGet().then(function(response) {
                    setUser(response.data as unknown as UserVM | null);
                    storageService.saveUserInfo(response.data as unknown as UserVM | null);
                });
                
            }).catch(function() {
                navigate("/");
            });
        })();
    }, []);

    function deleteAsset(arrayIndex: number) {
        if (assets === null || assets === undefined || assets.length === 0 || assets[arrayIndex] === undefined || assets[arrayIndex] === null || !assets[arrayIndex]) {
            return;
        }

        assetsApi.apiAssetsDelete(Number((assets as AssetVM[])[arrayIndex].id)).catch(function(error) {
            console.log(error);
        });
        
        const newAssets = [...assets];
        newAssets.splice(arrayIndex, 1);
        setAssets(newAssets);
    };

    function onChangeName(e: React.FormEvent<HTMLInputElement>): void {
        assetData.name = e.currentTarget.value;
    };

    function onChangeType(e: React.ChangeEvent<HTMLSelectElement>): void {
        assetData.type = e.currentTarget.value;
    };

    function onChangeQuantity(e: React.FormEvent<HTMLInputElement>): void {
        assetData.quantity = Number(e.currentTarget.value);
    };
    
    return (
        <>
            <Modal isOpen={modalOpen} overlayClassName="fixed inset-0 bg-black/20 z-50" className="w-[50%] h-[30%] mx-auto absolute left-[25%] top-[35%] z-50">
                <Form className="bg-white grid grid-cols-1 place-items-center w-full h-full">
                    <p className="text-2xl text-black">Add an asset</p>
                    <input 
                        className="m-0 font-bold rounded text-slate-500 bg-[#F4F4F4]"
                        type="text" 
                        placeholder="Name" 
                        onChange={onChangeName}
                    />

                    <input 
                        className="m-0 font-bold rounded text-slate-500 bg-[#F4F4F4]"
                        type="text"
                        placeholder="Quantity"
                        onChange={onChangeQuantity}
                    />
                    
                    <select 
                        className="m-0 font-bold rounded text-slate-500 bg-[#F4F4F4]"
                        onChange={onChangeType}
                    >
                        <option value="digital">Digital</option>
                        <option value="physical">Physical</option>
                    </select>

                    <button className="m-0 text-[#4ef542] transition-all duration-150 border-2 border-[#4ef542] rounded hover:bg-gray-100" id="sumbit" type="submit" onClick={() => {
                        assetsApi.apiAssetsPost(assetData.name, assetData.type, assetData.quantity).then(function(response) {
                            console.log(response);
                            const newAssets = [...(assets as AssetVM[])]
                            newAssets.push(response.data as unknown as AssetVM);
                            setAssets(newAssets);
                        }).catch(function(error) {
                            console.log(error);
                        });
                        
                        setModalOpen(false);
                    }}>
                        Add
                    </button>
                    <button className="absolute top-0 right-0 m-2 text-white bg-primary" onClick={() => {
                        setModalOpen(false);
                    }}>X</button>
                </Form>
            </Modal>
            
            <div className="relative top-0 left-0 w-screen h-[100%] min-h-[100%]">

                <div className="sticky top-0 left-0 bg-white w-screen p-3 z-40">
                    <nav className="w-[62%] mx-auto grid grid-cols-3 grid-rows-1 place-items-center">
                        <NavLink to="/assets" className="text-3xl text-black hover:text-primary hover:text-4xl transition-all duration-150 ease-in-out aria-[current=page]:text-4xl aria-[current=page]:text-primary aria-[current=page]:bg-transparent">Assets</NavLink>
                        <NavLink to="/home" className="text-3xl text-black hover:text-primary hover:text-4xl transition-all duration-150 ease-in-out aria-[current=page]:text-4xl aria-[current=page]:text-primary aria-[current=page]:bg-transparent">Home</NavLink>
                        <NavLink to="/wills" className="text-3xl text-black hover:text-primary hover:text-4xl transition-all duration-150 ease-in-out aria-[current=page]:text-4xl aria-[current=page]:text-primary aria-[current=page]:bg-transparent">Wills</NavLink>
                    </nav>
                </div>

                <div className="w-screen h-[calc(100vh-14rem)] bg-[#ECECEC]">
                    <div className="relative top-8 w-[62%] h-[calc(100vh-2*8rem)] mx-auto bg-white overflow-scroll">
                        {
                            assets?.map((element, index) => {
                                return (
                                    <div id={`asset${index}`} className="w-full h-fit p-4 bg-white border-4 flex justify-between items-center">
                                        <p className="text-2xl text-black">{element.name}</p>
                                        <button className="p-2 px-4 bg-white border-2 border-primary hover:bg-gray-100 text-primary rounded-md transition ease-in-out duration-100 h-fit" onClick={() => {
                                            deleteAsset(index);
                                        }}>Delete</button>
                                    </div>
                                );
                            })
                        }
                    </div>
                </div>

                <div className="bg-[#ECECEC] p-[3.4rem] w-screen h-fit">
                    <div className="relative top-[calc(48rem + 100vh-2*8.75rem)] w-[62%] h-fit mx-auto flex justify-center items-center">
                            <button className="p-2 px-4 bg-white border-2 rounded-md border-[#4ef542] text-[#4ef542] text-2xl hover:bg-gray-100 transition ease-in-out duration-100" onClick={() => {
                                setModalOpen(true);
                            }}>+</button>
                    </div>
                </div>

            </div>

            <Outlet/>
        </>
    );
}

export default Assets;