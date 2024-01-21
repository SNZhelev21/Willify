import globalAxios, { AxiosResponse, AxiosInstance, AxiosRequestConfig } from "axios";
import storageService from "../services/storage-service";

class UserApi {
    private axios: AxiosInstance;

    constructor(axios: AxiosInstance) {
        this.axios = axios;
    }

    public async apiUserGet(): Promise<AxiosResponse<void, any>> {
        const url = `http://willify.westeurope.cloudapp.azure.com:45098/user`;
        return await this.axios.get(url, {
            headers: {
                'Authorization': `Bearer ${storageService.retrieveAccessToken()}`
            }
        });
    }
}

const userApi = new UserApi(globalAxios);
export default userApi;