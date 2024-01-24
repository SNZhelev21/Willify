import globalAxios, { AxiosResponse, AxiosInstance, AxiosRequestConfig } from "axios";
import storageService from "../services/storage-service";

class TokenApi {
    private axios: AxiosInstance;

    constructor(axios: AxiosInstance) {
        this.axios = axios;
    }

    public async apiVerifyToken(): Promise<AxiosResponse<void, any>> {
        const url = `http://willify.westeurope.cloudapp.azure.com:45098/token`;
        const config = {
            headers: {
                'Authorization': `Bearer ${storageService.retrieveAccessToken()}`
            }
        };
        return await this.axios.post(url, "", config);
    }
}

const tokenApi = new TokenApi(globalAxios);
export default tokenApi;