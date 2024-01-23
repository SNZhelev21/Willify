import globalAxios, { AxiosResponse, AxiosInstance, AxiosRequestConfig } from "axios";
import storageService from "../services/storage-service";

class AssetsApi {
    private axios: AxiosInstance;

    constructor(axios: AxiosInstance) {
        this.axios = axios;
    }

    public async apiAssetsGetAll(): Promise<AxiosResponse<void, any>> {
        const url = `http://willify.westeurope.cloudapp.azure.com:45098/stocks`;
        return await this.axios.get(url, {
            headers: {
                'Authorization': `Bearer ${storageService.retrieveAccessToken()}`
            }
        });
    }

    public async apiAssetsGet(id: number): Promise<AxiosResponse<void, any>> {
        const url = `http://willify.westeurope.cloudapp.azure.com:45098/stock?id=${id}`;

        return await this.axios.get(url, {
            headers: {
                'Authorization': `Bearer ${storageService.retrieveAccessToken()}`
            }
        });
    }

    public async apiAssetsDelete(id: number): Promise<AxiosResponse<void, any>> {
        const url = 'http://willify.westeurope.cloudapp.azure.com:45098/stock';

        return await this.axios.delete(url, {
            headers: {
                'Authorization': `Bearer ${storageService.retrieveAccessToken()}`
            },
            data: {
                stock_id: `${id}`
            }
        })
    }

    public async apiAssetsPost(name: string): Promise<AxiosResponse<void, any>> {
        const url = 'http://willify.westeurope.cloudapp.azure.com:45098/stock';

        return await this.axios.post(url, {
            name: `${name}`
        }, {
            headers: {
                'Authorization': `Bearer ${storageService.retrieveAccessToken()}`
            }
        })
    }
}

const assetsApi = new AssetsApi(globalAxios);
export default assetsApi;