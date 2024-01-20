import globalAxios, { AxiosResponse, AxiosInstance, AxiosRequestConfig } from "axios";
import {UserLM} from "../models"
import {UserIM} from "../models"

class UserAuthApi {
    private axios: AxiosInstance;
    
    constructor(axios: AxiosInstance) {
        this.axios = axios;
    }
    
    public async apiUserAuthLoginPost(userLM: UserLM, config?: AxiosRequestConfig): Promise<AxiosResponse<void, any>> {
        const url = `http://willify-backend.germanywestcentral.cloudapp.azure.com:45098/login`;
        return await this.axios.post(url, JSON.stringify(userLM), {
            headers: {
              'Content-Type': 'application/json'
            }
          });
    }
    
    public async apiUserAuthRegisterPost(userIM: UserIM, config?: AxiosRequestConfig): Promise<AxiosResponse<void, any>> {
        const url = `http://willify-backend.germanywestcentral.cloudapp.azure.com:45098/register`;
        return await this.axios.post(url, JSON.stringify(userIM), {
            headers: {
              'Content-Type': 'application/json'
            }
          });
    }
}
const userAuthApi = new UserAuthApi(globalAxios);
export default userAuthApi;