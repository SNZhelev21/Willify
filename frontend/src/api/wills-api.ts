import globalAxios, { AxiosResponse, AxiosInstance, AxiosRequestConfig } from "axios";
import storageService from "../services/storage-service";

class WillsApi {
    private axios: AxiosInstance;

    constructor(axios: AxiosInstance) {
        this.axios = axios;
    }

    public async apiWillsGetAll(): Promise<AxiosResponse<void, any>> {
        const url = `http://willify.westeurope.cloudapp.azure.com:45098/wills`;
        return await this.axios.get(url, {
            headers: {
                'Authorization': `Bearer ${storageService.retrieveAccessToken()}`
            }
        });
    }

    public async apiWillsGet(id: Number): Promise<AxiosResponse<void, any>> {
        const url = `http://willify.westeurope.cloudapp.azure.com:45098/will?id=${id}`;

        return await this.axios.get(url, {
            headers: {
                'Authorization': `Bearer ${storageService.retrieveAccessToken()}`
            }
        });
    }

    public async apiWillsDelete(id: Number): Promise<AxiosResponse<void, any>> {
        const url = 'http://willify.westeurope.cloudapp.azure.com:45098/wills';

        return await this.axios.delete(url, {
            headers: {
                'Authorization': `Bearer ${storageService.retrieveAccessToken()}`
            },
            data: {
                will_id: `${id}`
            }
        })
    }

    public async apiWillsPost(stockId: String, beneficiaryName: String, beneficiaryRel: String, quantity: Number): Promise<AxiosResponse<void, any>> {
        const url = 'http://willify.westeurope.cloudapp.azure.com:45098/will';

        return await this.axios.post(url, {
            stock_id: `${stockId}`,
            beneficiary_name: `${beneficiaryName}`,
            beneficiary_relation: `${beneficiaryRel}`,
            quantity: `${quantity}`
        }, {
            headers: {
                'Authorization': `Bearer ${storageService.retrieveAccessToken()}`
            }
        })
    }
}

const willsApi = new WillsApi(globalAxios);
export default willsApi;